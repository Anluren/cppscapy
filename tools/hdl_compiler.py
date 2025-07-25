#!/usr/bin/env python3
"""
Header DSL Compiler
===================

A compiler for the Network Header Domain Specific Language (HDL) that generates
C++ header classes with precise bit field manipulation.

Usage:
    python3 hdl_compiler.py input.hdl -o output.h
    python3 hdl_compiler.py --help

Requirements:
    Python 3.7+ (for dataclasses support)
    Python 3.8+ recommended for better performance and features
"""

import argparse
import re
import sys
import os
from pathlib import Path
import xml.etree.ElementTree as ET
from xml.dom import minidom

# Check Python version
if sys.version_info < (3, 7):
    print("Error: This script requires Python 3.7 or higher", file=sys.stderr)
    print(f"Current version: {sys.version}", file=sys.stderr)
    sys.exit(1)
from typing import Dict, List, Optional, Tuple, Union
from dataclasses import dataclass
from enum import Enum


class FieldType(Enum):
    INTEGER = "integer"
    ENUM = "enum"
    ARRAY = "array"
    UNION = "union"
    VARIABLE = "variable"


@dataclass
class Field:
    name: str
    bit_width: Union[int, str]  # Can be integer or expression
    field_type: FieldType = FieldType.INTEGER
    default_value: Optional[Union[int, str]] = None
    attributes: List[str] = None
    enum_type: Optional[str] = None
    condition: Optional[str] = None
    
    def __post_init__(self):
        if self.attributes is None:
            self.attributes = []


@dataclass
class EnumValue:
    name: str
    value: Union[int, str]


@dataclass
class EnumDef:
    name: str
    underlying_type: str
    values: List[EnumValue]


@dataclass
class HeaderDef:
    name: str
    fields: List[Field]
    attributes: List[str] = None
    
    def __post_init__(self):
        if self.attributes is None:
            self.attributes = []


class HDLParser:
    """Parser for Header Definition Language (HDL)"""
    
    def __init__(self):
        self.enums: Dict[str, EnumDef] = {}
        self.headers: Dict[str, HeaderDef] = {}
    
    def parse_file(self, filename: str) -> None:
        """Parse an HDL file"""
        with open(filename, 'r') as f:
            content = f.read()
        
        self.parse_content(content)
    
    def parse_content(self, content: str) -> None:
        """Parse HDL content"""
        # Remove comments
        content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
        
        # Parse enums
        enum_pattern = r'enum\s+(\w+)\s*:\s*(\w+)\s*\{([^}]+)\}'
        for match in re.finditer(enum_pattern, content):
            enum_name = match.group(1)
            underlying_type = match.group(2)
            enum_body = match.group(3)
            
            self.enums[enum_name] = self._parse_enum(enum_name, underlying_type, enum_body)
        
        # Parse headers
        header_pattern = r'header\s+(\w+)(?:\s*\[([^\]]+)\])?\s*\{([^}]+)\}'
        for match in re.finditer(header_pattern, content):
            header_name = match.group(1)
            attributes_str = match.group(2) or ""
            header_body = match.group(3)
            
            attributes = [attr.strip() for attr in attributes_str.split(',') if attr.strip()]
            self.headers[header_name] = self._parse_header(header_name, header_body, attributes)
    
    def _parse_enum(self, name: str, underlying_type: str, body: str) -> EnumDef:
        """Parse enum definition"""
        values = []
        
        for line in body.split(','):
            line = line.strip()
            if not line:
                continue
            
            if '=' in line:
                value_name, value_expr = line.split('=', 1)
                value_name = value_name.strip()
                value_expr = value_expr.strip()
                
                # Try to parse as integer, otherwise keep as string
                try:
                    if value_expr.startswith('0x'):
                        value = int(value_expr, 16)
                    else:
                        value = int(value_expr)
                except ValueError:
                    value = value_expr
                
                values.append(EnumValue(value_name, value))
            else:
                values.append(EnumValue(line, len(values)))
        
        return EnumDef(name, underlying_type, values)
    
    def _parse_header(self, name: str, body: str, attributes: List[str]) -> HeaderDef:
        """Parse header definition"""
        fields = []
        
        for line in body.split(';'):
            line = line.strip()
            if not line:
                continue
            
            field = self._parse_field(line)
            if field:
                fields.append(field)
        
        return HeaderDef(name, fields, attributes)
    
    def _parse_field(self, line: str) -> Optional[Field]:
        """Parse a single field definition"""
        # Basic field pattern: name: width [= default] [attributes]
        match = re.match(r'(\w+)\s*:\s*([^=\[]+)(?:\s*=\s*([^=\[]+))?(?:\s*\[([^\]]+)\])?', line)
        if not match:
            return None
        
        field_name = match.group(1)
        width_str = match.group(2).strip()
        default_str = match.group(3)
        attributes_str = match.group(4) or ""
        
        # Parse width
        try:
            bit_width = int(width_str)
        except ValueError:
            bit_width = width_str  # Keep as expression
        
        # Parse default value
        default_value = None
        if default_str:
            default_str = default_str.strip()
            try:
                if default_str.startswith('0x'):
                    default_value = int(default_str, 16)
                else:
                    default_value = int(default_str)
            except ValueError:
                default_value = default_str
        
        # Parse attributes
        attributes = []
        if attributes_str:
            attributes = [attr.strip() for attr in attributes_str.split(',')]
        
        # Determine field type
        field_type = FieldType.INTEGER
        enum_type = None
        
        if width_str in self.enums:
            field_type = FieldType.ENUM
            enum_type = width_str
            bit_width = self._get_enum_bit_width(width_str)
        
        return Field(
            name=field_name,
            bit_width=bit_width,
            field_type=field_type,
            default_value=default_value,
            attributes=attributes,
            enum_type=enum_type
        )
    
    def _get_enum_bit_width(self, enum_name: str) -> int:
        """Get bit width for enum based on underlying type"""
        enum_def = self.enums[enum_name]
        type_widths = {
            'uint8_t': 8, 'uint16_t': 16, 'uint32_t': 32, 'uint64_t': 64,
            '8': 8, '16': 16, '32': 32, '64': 64
        }
        return type_widths.get(enum_def.underlying_type, 16)


class XMLParser:
    """Parser for XML-based Network Protocol Definition"""
    
    def __init__(self):
        self.enums: Dict[str, EnumDef] = {}
        self.headers: Dict[str, HeaderDef] = {}
    
    def parse_file(self, filename: str) -> None:
        """Parse an XML file"""
        try:
            tree = ET.parse(filename)
            root = tree.getroot()
            self.parse_xml_content(root)
        except ET.ParseError as e:
            raise ValueError(f"XML parsing error: {e}")
        except Exception as e:
            raise ValueError(f"Error reading XML file: {e}")
    
    def parse_xml_content(self, root: ET.Element) -> None:
        """Parse XML content from root element"""
        # Parse enums
        for enum_elem in root.findall('enum'):
            enum_def = self._parse_xml_enum(enum_elem)
            self.enums[enum_def.name] = enum_def
        
        # Parse headers
        for header_elem in root.findall('header'):
            header_def = self._parse_xml_header(header_elem)
            self.headers[header_def.name] = header_def
    
    def _parse_xml_enum(self, enum_elem: ET.Element) -> EnumDef:
        """Parse enum from XML element"""
        name = enum_elem.get('name', '')
        underlying_type = enum_elem.get('underlying_type', 'uint16_t')
        
        values = []
        for value_elem in enum_elem.findall('value'):
            value_name = value_elem.get('name', '')
            value_str = value_elem.get('value', '0')
            
            # Parse value
            try:
                if value_str.startswith('0x'):
                    value = int(value_str, 16)
                else:
                    value = int(value_str)
            except ValueError:
                value = value_str
            
            values.append(EnumValue(value_name, value))
        
        return EnumDef(name, underlying_type, values)
    
    def _parse_xml_header(self, header_elem: ET.Element) -> HeaderDef:
        """Parse header from XML element"""
        name = header_elem.get('name', '')
        
        # Parse attributes (if any are defined at header level)
        attributes = []
        
        fields = []
        for field_elem in header_elem.findall('field'):
            field = self._parse_xml_field(field_elem)
            if field:
                fields.append(field)
        
        return HeaderDef(name, fields, attributes)
    
    def _parse_xml_field(self, field_elem: ET.Element) -> Optional[Field]:
        """Parse field from XML element"""
        field_name = field_elem.get('name', '')
        bit_width_str = field_elem.get('bit_width', '0')
        field_type_str = field_elem.get('type', 'integer')
        enum_type = field_elem.get('enum_type')
        default_str = field_elem.get('default')
        
        # Parse bit width
        try:
            bit_width = int(bit_width_str)
        except ValueError:
            bit_width = bit_width_str  # Keep as expression
        
        # Parse default value
        default_value = None
        if default_str:
            try:
                if default_str.startswith('0x'):
                    default_value = int(default_str, 16)
                else:
                    default_value = int(default_str)
            except ValueError:
                default_value = default_str
        
        # Parse attributes
        attributes = []
        attr_elem = field_elem.find('attributes')
        if attr_elem is not None:
            for attr in attr_elem.findall('attribute'):
                if attr.text:
                    attributes.append(attr.text.strip())
        
        # Determine field type
        field_type = FieldType.INTEGER
        if field_type_str == 'enum':
            field_type = FieldType.ENUM
            if enum_type and enum_type in self.enums:
                bit_width = self._get_enum_bit_width(enum_type)
        elif field_type_str == 'array':
            field_type = FieldType.ARRAY
        elif field_type_str == 'union':
            field_type = FieldType.UNION
        elif field_type_str == 'variable':
            field_type = FieldType.VARIABLE
        
        return Field(
            name=field_name,
            bit_width=bit_width,
            field_type=field_type,
            default_value=default_value,
            attributes=attributes,
            enum_type=enum_type
        )
    
    def _get_enum_bit_width(self, enum_name: str) -> int:
        """Get bit width for enum based on underlying type"""
        enum_def = self.enums[enum_name]
        type_widths = {
            'uint8_t': 8, 'uint16_t': 16, 'uint32_t': 32, 'uint64_t': 64,
            '8': 8, '16': 16, '32': 32, '64': 64
        }
        return type_widths.get(enum_def.underlying_type, 16)


class UniversalParser:
    """Universal parser that can handle both HDL and XML formats"""
    
    def __init__(self):
        self.enums: Dict[str, EnumDef] = {}
        self.headers: Dict[str, HeaderDef] = {}
    
    def parse_file(self, filename: str) -> None:
        """Parse a file, auto-detecting format based on extension"""
        file_ext = Path(filename).suffix.lower()
        
        if file_ext == '.xml':
            parser = XMLParser()
        elif file_ext == '.hdl':
            parser = HDLParser()
        else:
            # Try to detect format by content
            with open(filename, 'r') as f:
                content = f.read(1024)  # Read first 1KB
            
            if content.strip().startswith('<?xml') or '<network_protocols' in content:
                parser = XMLParser()
            else:
                parser = HDLParser()
        
        parser.parse_file(filename)
        self.enums = parser.enums
        self.headers = parser.headers


class CPPCodeGenerator:
    """Generate C++ code from parsed HDL or XML"""
    
    def __init__(self, parser: Union[HDLParser, XMLParser, UniversalParser], namespace: str = "cppscapy::dsl"):
        self.parser = parser
        self.namespace = namespace
        self.output = []
    
    def generate(self) -> str:
        """Generate complete C++ header file"""
        self.output = []
        
        self._generate_header()
        self._generate_includes()
        self._generate_namespace_open()
        self._generate_base_classes()
        self._generate_enums()
        self._generate_headers()
        self._generate_namespace_close()
        
        return '\n'.join(self.output)
    
    def _generate_header(self):
        """Generate file header"""
        self.output.extend([
            "#pragma once",
            "// Generated by HDL Compiler",
            "// Do not edit manually",
            ""
        ])
    
    def _generate_includes(self):
        """Generate include statements"""
        self.output.extend([
            "#include <cstdint>",
            "#include <vector>",
            "#include <bitset>",
            "#include <string>",
            "#include <type_traits>",
            ""
        ])
    
    def _generate_namespace_open(self):
        """Generate namespace opening"""
        self.output.extend([
            f"namespace {self.namespace} {{",
            ""
        ])
    
    def _generate_namespace_close(self):
        """Generate namespace closing"""
        self.output.extend([
            "",
            f"}} // namespace {self.namespace}"
        ])
    
    def _generate_base_classes(self):
        """Generate base classes and utilities"""
        self.output.extend([
            "// Base class for all generated headers",
            "class HeaderBase {",
            "public:",
            "    virtual ~HeaderBase() = default;",
            "    ",
            "    virtual std::vector<uint8_t> to_bytes() const = 0;",
            "    virtual bool from_bytes(const std::vector<uint8_t>& data) = 0;",
            "    virtual size_t size_bits() const = 0;",
            "    virtual size_t size_bytes() const { return (size_bits() + 7) / 8; }",
            "    virtual bool is_valid() const = 0;",
            "    virtual void update_computed_fields() = 0;",
            "};",
            "",
            "// Bit field manipulation utilities",
            "template<typename T>",
            "class BitField {",
            "public:",
            "    BitField(std::vector<uint8_t>& data, size_t bit_offset, size_t bit_width)",
            "        : data_(data), bit_offset_(bit_offset), bit_width_(bit_width) {}",
            "    ",
            "    T get() const {",
            "        T value = 0;",
            "        for (size_t i = 0; i < bit_width_; ++i) {",
            "            size_t byte_idx = (bit_offset_ + i) / 8;",
            "            size_t bit_idx = (bit_offset_ + i) % 8;",
            "            ",
            "            if (byte_idx < data_.size()) {",
            "                uint8_t bit = (data_[byte_idx] >> (7 - bit_idx)) & 1;",
            "                value |= (static_cast<T>(bit) << (bit_width_ - 1 - i));",
            "            }",
            "        }",
            "        return value;",
            "    }",
            "    ",
            "    void set(T value) {",
            "        // Ensure data vector is large enough",
            "        size_t required_bytes = (bit_offset_ + bit_width_ + 7) / 8;",
            "        if (data_.size() < required_bytes) {",
            "            data_.resize(required_bytes, 0);",
            "        }",
            "        ",
            "        for (size_t i = 0; i < bit_width_; ++i) {",
            "            size_t byte_idx = (bit_offset_ + i) / 8;",
            "            size_t bit_idx = (bit_offset_ + i) % 8;",
            "            ",
            "            uint8_t bit = (value >> (bit_width_ - 1 - i)) & 1;",
            "            ",
            "            // Clear the bit first",
            "            data_[byte_idx] &= ~(1 << (7 - bit_idx));",
            "            // Set the bit if needed",
            "            data_[byte_idx] |= (bit << (7 - bit_idx));",
            "        }",
            "    }",
            "    ",
            "    operator T() const { return get(); }",
            "    BitField& operator=(T value) { set(value); return *this; }",
            "    ",
            "private:",
            "    std::vector<uint8_t>& data_;",
            "    size_t bit_offset_;",
            "    size_t bit_width_;",
            "};",
            ""
        ])
    
    def _generate_enums(self):
        """Generate enum definitions"""
        for enum_name, enum_def in self.parser.enums.items():
            self.output.append(f"// Generated enum: {enum_name}")
            self.output.append(f"enum class {enum_name} : {enum_def.underlying_type} {{")
            
            for value in enum_def.values:
                if isinstance(value.value, int):
                    if value.value > 255:
                        self.output.append(f"    {value.name} = 0x{value.value:04X},")
                    else:
                        self.output.append(f"    {value.name} = {value.value},")
                else:
                    self.output.append(f"    {value.name} = {value.value},")
            
            self.output.append("};")
            self.output.append("")
    
    def _generate_headers(self):
        """Generate header class definitions"""
        for header_name, header_def in self.parser.headers.items():
            self._generate_single_header(header_name, header_def)
    
    def _generate_single_header(self, name: str, header: HeaderDef):
        """Generate a single header class"""
        total_bits = self._calculate_total_bits(header.fields)
        total_bytes = (total_bits + 7) // 8
        
        self.output.extend([
            f"// Generated from DSL: header {name}",
            f"class {name} : public HeaderBase {{",
            "public:",
            f"    {name}() : data_({total_bytes}, 0) {{}}"
        ])
        
        # Generate field accessors
        bit_offset = 0
        for field in header.fields:
            if isinstance(field.bit_width, int):
                self._generate_field_accessors(field, bit_offset)
                bit_offset += field.bit_width
        
        # Generate HeaderBase implementation
        self.output.extend([
            "    ",
            "    // HeaderBase implementation",
            "    std::vector<uint8_t> to_bytes() const override { return data_; }",
            "    ",
            "    bool from_bytes(const std::vector<uint8_t>& data) override {",
            f"        if (data.size() < {total_bytes}) return false;",
            "        data_ = data;",
            f"        data_.resize({total_bytes});",
            "        return true;",
            "    }",
            "    ",
            f"    size_t size_bits() const override {{ return {total_bits}; }}",
            f"    bool is_valid() const override {{ return data_.size() == {total_bytes}; }}",
            "    ",
            "    void update_computed_fields() override {",
        ])
        
        # Generate computed field updates
        for field in header.fields:
            if "computed" in field.attributes:
                if field.name == "length":
                    self.output.append(f"        // Update {field.name} field")
                    self.output.append(f"        // TODO: Implement length calculation")
                elif field.name == "checksum":
                    self.output.append(f"        // Update {field.name} field")
                    self.output.append(f"        // TODO: Implement checksum calculation")
        
        self.output.extend([
            "    }",
            "    ",
            "private:",
            "    std::vector<uint8_t> data_;",
            "};",
            ""
        ])
    
    def _generate_field_accessors(self, field: Field, bit_offset: int):
        """Generate getter and setter for a field"""
        cpp_type = self._get_cpp_type(field)
        bit_width = field.bit_width
        
        if field.field_type == FieldType.ENUM:
            # Enum field accessors
            self.output.extend([
                f"    ",
                f"    // {field.name} field ({bit_width} bits)",
                f"    {field.enum_type} {field.name}() const {{",
                f"        return static_cast<{field.enum_type}>(BitField<{self._get_underlying_type(field)}>(const_cast<std::vector<uint8_t>&>(data_), {bit_offset}, {bit_width}).get());",
                f"    }}",
                f"    void set_{field.name}({field.enum_type} value) {{",
                f"        BitField<{self._get_underlying_type(field)}>(data_, {bit_offset}, {bit_width}).set(static_cast<{self._get_underlying_type(field)}>(value));",
                f"    }}"
            ])
        else:
            # Regular field accessors
            self.output.extend([
                f"    ",
                f"    // {field.name} field ({bit_width} bits)",
                f"    {cpp_type} {field.name}() const {{",
                f"        return BitField<{cpp_type}>(const_cast<std::vector<uint8_t>&>(data_), {bit_offset}, {bit_width}).get();",
                f"    }}",
                f"    void set_{field.name}({cpp_type} value) {{",
                f"        BitField<{cpp_type}>(data_, {bit_offset}, {bit_width}).set(value);",
                f"    }}"
            ])
    
    def _get_cpp_type(self, field: Field) -> str:
        """Get appropriate C++ type for field"""
        if field.field_type == FieldType.ENUM:
            return field.enum_type
        
        if isinstance(field.bit_width, str):
            return "uint32_t"  # Default for variable width
        
        bit_width = field.bit_width
        if bit_width <= 8:
            return "uint8_t"
        elif bit_width <= 16:
            return "uint16_t"
        elif bit_width <= 32:
            return "uint32_t"
        else:
            return "uint64_t"
    
    def _get_underlying_type(self, field: Field) -> str:
        """Get underlying type for enum field"""
        if field.enum_type and field.enum_type in self.parser.enums:
            return self.parser.enums[field.enum_type].underlying_type
        return "uint16_t"
    
    def _calculate_total_bits(self, fields: List[Field]) -> int:
        """Calculate total bits for all fields"""
        total = 0
        for field in fields:
            if isinstance(field.bit_width, int):
                total += field.bit_width
        return total


# Validation functions for command-line arguments
def is_valid_filename(filename: str) -> bool:
    """Cross-platform filename validation using multiple approaches"""
    if not filename:
        return False
    
    # Method 1: Try to create a Path object and resolve it
    try:
        path = Path(filename)
        path.resolve()
    except (OSError, ValueError):
        return False
    
    # Method 2: Check for forbidden characters (platform-specific)
    import platform
    if platform.system() == 'Windows':
        # Windows forbidden characters
        forbidden = '<>:"|?*'
        if any(char in filename for char in forbidden):
            return False
        # Windows reserved names
        reserved = {'CON', 'PRN', 'AUX', 'NUL', 'COM1', 'COM2', 'COM3', 'COM4', 
                   'COM5', 'COM6', 'COM7', 'COM8', 'COM9', 'LPT1', 'LPT2', 
                   'LPT3', 'LPT4', 'LPT5', 'LPT6', 'LPT7', 'LPT8', 'LPT9'}
        if Path(filename).stem.upper() in reserved:
            return False
    else:
        # Unix-like systems: only null character is forbidden
        if '\0' in filename:
            return False
    
    # Method 3: Check length limits
    if len(filename) > 255:  # Most filesystems have 255 character limit
        return False
    
    return True


def validate_input_file(filename: str) -> str:
    """Validate input HDL or XML file"""
    if not filename:
        raise argparse.ArgumentTypeError("Input file cannot be empty")
    
    try:
        # Use pathlib for robust path validation
        path = Path(filename)
        
        # Check if it's a valid path (this will raise an exception for invalid characters)
        path.resolve()
        
    except (OSError, ValueError) as e:
        raise argparse.ArgumentTypeError(
            f"Input file '{filename}' contains invalid path characters: {e}"
        )
    
    # Check file extension
    if not filename.lower().endswith(('.hdl', '.HDL', '.xml', '.XML')):
        raise argparse.ArgumentTypeError(
            f"Input file '{filename}' must have .hdl or .xml extension"
        )
    
    # Check if file exists
    if not path.exists():
        raise argparse.ArgumentTypeError(f"Input file '{filename}' does not exist")
    
    # Check if it's actually a file (not a directory)
    if not path.is_file():
        raise argparse.ArgumentTypeError(f"'{filename}' is not a file")
    
    # Check if file is readable
    if not os.access(filename, os.R_OK):
        raise argparse.ArgumentTypeError(f"Input file '{filename}' is not readable")
    
    return filename


def validate_output_file(filename: str) -> str:
    """Validate output C++ header file"""
    if not filename:
        raise argparse.ArgumentTypeError("Output file cannot be empty")
    
    try:
        # Use pathlib for robust path validation
        path = Path(filename)
        
        # Check if it's a valid path (this will raise an exception for invalid characters)
        path.resolve()
        
    except (OSError, ValueError) as e:
        raise argparse.ArgumentTypeError(
            f"Output file '{filename}' contains invalid path characters: {e}"
        )
    
    # Check file extension
    if not filename.lower().endswith(('.h', '.hpp', '.hxx', '.H')):
        raise argparse.ArgumentTypeError(
            f"Output file '{filename}' must have a C++ header extension (.h, .hpp, .hxx)"
        )
    
    # Check if directory exists (if path contains directory)
    output_dir = path.parent
    if output_dir != Path('.') and not output_dir.exists():
        raise argparse.ArgumentTypeError(
            f"Output directory '{output_dir}' does not exist"
        )
    
    # Check if we can write to the directory
    if output_dir.exists() and not os.access(output_dir, os.W_OK):
        raise argparse.ArgumentTypeError(
            f"Cannot write to output directory '{output_dir}'"
        )
    elif output_dir == Path('.') and not os.access('.', os.W_OK):
        raise argparse.ArgumentTypeError(
            "Cannot write to current directory"
        )
    
    return filename


def validate_namespace(namespace: str) -> str:
    """Validate C++ namespace"""
    if not namespace:
        raise argparse.ArgumentTypeError("Namespace cannot be empty")
    
    # Check for valid C++ namespace characters and format
    namespace_pattern = r'^[a-zA-Z_][a-zA-Z0-9_]*(?:::[a-zA-Z_][a-zA-Z0-9_]*)*$'
    if not re.match(namespace_pattern, namespace):
        raise argparse.ArgumentTypeError(
            f"Namespace '{namespace}' is not a valid C++ namespace. "
            "Must contain only alphanumeric characters, underscores, and :: separators. "
            "Cannot start with numbers."
        )
    
    # Check for C++ reserved keywords
    cpp_keywords = {
        'auto', 'break', 'case', 'char', 'const', 'continue', 'default', 'do',
        'double', 'else', 'enum', 'extern', 'float', 'for', 'goto', 'if',
        'int', 'long', 'register', 'return', 'short', 'signed', 'sizeof', 'static',
        'struct', 'switch', 'typedef', 'union', 'unsigned', 'void', 'volatile', 'while',
        'class', 'delete', 'friend', 'inline', 'new', 'operator', 'private',
        'protected', 'public', 'template', 'this', 'virtual', 'namespace', 'using',
        'try', 'catch', 'throw', 'const_cast', 'dynamic_cast', 'explicit',
        'export', 'false', 'mutable', 'reinterpret_cast', 'static_cast', 'true',
        'typeid', 'typename', 'wchar_t', 'and', 'bitand', 'bitor', 'compl',
        'not', 'or', 'xor', 'and_eq', 'not_eq', 'or_eq', 'xor_eq'
    }
    
    # Split namespace and check each part
    parts = namespace.split('::')
    for part in parts:
        if part.lower() in cpp_keywords:
            raise argparse.ArgumentTypeError(
                f"Namespace part '{part}' is a C++ reserved keyword"
            )
    
    return namespace


def main():
    parser = argparse.ArgumentParser(
        description="Header Definition Language (HDL) Compiler",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    hdl_compiler.py protocols.hdl -o generated_headers.h
    hdl_compiler.py network_definitions.xml --output include/network.h
    hdl_compiler.py --namespace my::protocols input.xml -o output.h

Constraints:
    - Input file must have .hdl or .xml extension and exist
    - Output file must have C++ header extension (.h, .hpp, .hxx)
    - Namespace must be valid C++ identifier(s) separated by ::
    - Filenames can only contain alphanumeric, dots, underscores, hyphens, and path separators
        """
    )
    
    parser.add_argument(
        "input_file",
        type=validate_input_file,
        help="Input HDL or XML file to compile (must have .hdl or .xml extension)"
    )
    
    parser.add_argument(
        "-o", "--output",
        type=validate_output_file,
        help="Output C++ header file (must have .h/.hpp/.hxx extension, default: input filename with .h extension)"
    )
    
    parser.add_argument(
        "--namespace",
        type=validate_namespace,
        default="cppscapy::dsl",
        help="C++ namespace for generated code (default: cppscapy::dsl)"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    # Add version information
    parser.add_argument(
        "--version",
        action="version",
        version="HDL Compiler 1.0.0 (Python {}.{}.{})".format(*sys.version_info[:3])
    )
    
    try:
        args = parser.parse_args()
    except argparse.ArgumentTypeError as e:
        parser.error(str(e))
    
    # Determine output filename if not provided
    if args.output:
        output_file = args.output
    else:
        # Validate the auto-generated output filename
        base_name = args.input_file.rsplit('.', 1)[0]
        output_file = base_name + '.h'
        try:
            output_file = validate_output_file(output_file)
        except argparse.ArgumentTypeError as e:
            parser.error(f"Auto-generated output filename invalid: {e}")
    
    try:
        # Parse HDL/XML file
        if args.verbose:
            print(f"Parsing {args.input_file}...")
            print(f"Output namespace: {args.namespace}")
            print(f"Output file: {output_file}")
        
        parser = UniversalParser()
        parser.parse_file(args.input_file)
        
        if args.verbose:
            print(f"Found {len(parser.enums)} enums and {len(parser.headers)} headers")
        
        # Validate that we found some content
        if not parser.enums and not parser.headers:
            print("Warning: No enums or headers found in input file", file=sys.stderr)
        
        # Generate C++ code
        if args.verbose:
            print(f"Generating C++ code...")
        
        generator = CPPCodeGenerator(parser, args.namespace)
        cpp_code = generator.generate()
        
        # Write output file with error handling
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(cpp_code)
        except IOError as e:
            print(f"Error writing output file '{output_file}': {e}", file=sys.stderr)
            sys.exit(1)
        
        if args.verbose:
            print(f"Generated {output_file}")
            print(f"File size: {len(cpp_code)} characters")
        else:
            print(f"Successfully compiled {args.input_file} -> {output_file}")
    
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except PermissionError as e:
        print(f"Error: Permission denied - {e}", file=sys.stderr)
        sys.exit(1)
    except UnicodeDecodeError as e:
        print(f"Error: Invalid file encoding - {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
