#!/usr/bin/env python3
"""
Header DSL Compiler
===================

A compiler for the Network Header Domain Specific Language (HDL) that generates
C++ header classes with precise bit field manipulation.

Usage:
    python3 hdl_compiler.py input.hdl -o output.h
    python3 hdl_compiler.py --help
"""

import argparse
import re
import sys
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


class CPPCodeGenerator:
    """Generate C++ code from parsed HDL"""
    
    def __init__(self, parser: HDLParser):
        self.parser = parser
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
            "namespace cppscapy::dsl {",
            ""
        ])
    
    def _generate_namespace_close(self):
        """Generate namespace closing"""
        self.output.extend([
            "",
            "} // namespace cppscapy::dsl"
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


def main():
    parser = argparse.ArgumentParser(
        description="Header Definition Language (HDL) Compiler",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    hdl_compiler.py example.hdl -o generated_headers.h
    hdl_compiler.py --input protocols.hdl --output network_headers.h
        """
    )
    
    parser.add_argument(
        "input_file",
        help="Input HDL file to compile"
    )
    
    parser.add_argument(
        "-o", "--output",
        help="Output C++ header file (default: input filename with .h extension)"
    )
    
    parser.add_argument(
        "--namespace",
        default="cppscapy::dsl",
        help="C++ namespace for generated code (default: cppscapy::dsl)"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    # Determine output filename
    if args.output:
        output_file = args.output
    else:
        output_file = args.input_file.rsplit('.', 1)[0] + '.h'
    
    try:
        # Parse HDL file
        if args.verbose:
            print(f"Parsing {args.input_file}...")
        
        hdl_parser = HDLParser()
        hdl_parser.parse_file(args.input_file)
        
        if args.verbose:
            print(f"Found {len(hdl_parser.enums)} enums and {len(hdl_parser.headers)} headers")
        
        # Generate C++ code
        if args.verbose:
            print(f"Generating C++ code...")
        
        generator = CPPCodeGenerator(hdl_parser)
        cpp_code = generator.generate()
        
        # Write output file
        with open(output_file, 'w') as f:
            f.write(cpp_code)
        
        if args.verbose:
            print(f"Generated {output_file}")
        else:
            print(f"Successfully compiled {args.input_file} -> {output_file}")
    
    except FileNotFoundError:
        print(f"Error: Input file '{args.input_file}' not found", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
