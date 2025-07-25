#!/usr/bin/env python3
"""
Format Converter for HDL Compiler
================================

Convert between HDL and XML formats for network protocol definitions.

Usage:
    python3 format_converter.py input.hdl -o output.xml
    python3 format_converter.py input.xml -o output.hdl
"""

import argparse
import sys
import os
from pathlib import Path
import xml.etree.ElementTree as ET
from xml.dom import minidom

# Import the existing parser classes
sys.path.append(os.path.dirname(__file__))
from hdl_compiler import HDLParser, XMLParser, EnumDef, HeaderDef, Field, FieldType

class FormatConverter:
    """Convert between HDL and XML formats"""
    
    def __init__(self):
        self.enums = {}
        self.headers = {}
    
    def convert_file(self, input_file: str, output_file: str) -> None:
        """Convert file from one format to another"""
        input_ext = Path(input_file).suffix.lower()
        output_ext = Path(output_file).suffix.lower()
        
        # Parse input
        if input_ext == '.hdl':
            parser = HDLParser()
        elif input_ext == '.xml':
            parser = XMLParser()
        else:
            raise ValueError(f"Unsupported input format: {input_ext}")
        
        parser.parse_file(input_file)
        self.enums = parser.enums
        self.headers = parser.headers
        
        # Generate output
        if output_ext == '.hdl':
            content = self._generate_hdl()
        elif output_ext == '.xml':
            content = self._generate_xml()
        else:
            raise ValueError(f"Unsupported output format: {output_ext}")
        
        # Write output
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(content)
    
    def _generate_hdl(self) -> str:
        """Generate HDL format content"""
        lines = []
        lines.append("// Converted from XML format")
        lines.append("// Network Protocol Headers Definition")
        lines.append("")
        
        # Generate enums
        for enum_name, enum_def in self.enums.items():
            lines.append(f"enum {enum_name} : {enum_def.underlying_type} {{")
            for value in enum_def.values:
                if isinstance(value.value, int):
                    if value.value > 255:
                        lines.append(f"    {value.name} = 0x{value.value:04X},")
                    else:
                        lines.append(f"    {value.name} = {value.value},")
                else:
                    lines.append(f"    {value.name} = {value.value},")
            lines.append("}")
            lines.append("")
        
        # Generate headers
        for header_name, header_def in self.headers.items():
            if header_def.attributes:
                attr_str = "[" + ", ".join(header_def.attributes) + "]"
                lines.append(f"header {header_name} {attr_str} {{")
            else:
                lines.append(f"header {header_name} {{")
            
            for field in header_def.fields:
                field_line = f"    {field.name}: "
                
                if field.field_type == FieldType.ENUM and field.enum_type:
                    field_line += field.enum_type
                else:
                    field_line += str(field.bit_width)
                
                if field.default_value is not None:
                    if isinstance(field.default_value, int):
                        if field.default_value > 255:
                            field_line += f" = 0x{field.default_value:04X}"
                        else:
                            field_line += f" = {field.default_value}"
                    else:
                        field_line += f" = {field.default_value}"
                
                if field.attributes:
                    attr_str = "[" + ", ".join(field.attributes) + "]"
                    field_line += f" {attr_str}"
                
                field_line += ";"
                lines.append(field_line)
            
            lines.append("}")
            lines.append("")
        
        return "\n".join(lines)
    
    def _generate_xml(self) -> str:
        """Generate XML format content"""
        root = ET.Element("network_protocols")
        root.set("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
        root.set("xsi:noNamespaceSchemaLocation", "network_protocols.xsd")
        
        # Add comment
        root.append(ET.Comment(" Converted from HDL format "))
        
        # Generate enums
        for enum_name, enum_def in self.enums.items():
            enum_elem = ET.SubElement(root, "enum")
            enum_elem.set("name", enum_name)
            enum_elem.set("underlying_type", enum_def.underlying_type)
            
            for value in enum_def.values:
                value_elem = ET.SubElement(enum_elem, "value")
                value_elem.set("name", value.name)
                if isinstance(value.value, int):
                    if value.value > 255:
                        value_elem.set("value", f"0x{value.value:04X}")
                    else:
                        value_elem.set("value", str(value.value))
                else:
                    value_elem.set("value", str(value.value))
        
        # Generate headers
        for header_name, header_def in self.headers.items():
            header_elem = ET.SubElement(root, "header")
            header_elem.set("name", header_name)
            header_elem.set("description", f"Generated {header_name} protocol header")
            
            for field in header_def.fields:
                field_elem = ET.SubElement(header_elem, "field")
                field_elem.set("name", field.name)
                field_elem.set("bit_width", str(field.bit_width))
                field_elem.set("description", f"{field.name.replace('_', ' ').title()} field")
                
                if field.field_type == FieldType.ENUM:
                    field_elem.set("type", "enum")
                    if field.enum_type:
                        field_elem.set("enum_type", field.enum_type)
                else:
                    field_elem.set("type", "integer")
                
                if field.default_value is not None:
                    if isinstance(field.default_value, int):
                        if field.default_value > 255:
                            field_elem.set("default", f"0x{field.default_value:04X}")
                        else:
                            field_elem.set("default", str(field.default_value))
                    else:
                        field_elem.set("default", str(field.default_value))
                
                # Add attributes if any
                if field.attributes:
                    attrs_elem = ET.SubElement(field_elem, "attributes")
                    for attr in field.attributes:
                        attr_elem = ET.SubElement(attrs_elem, "attribute")
                        attr_elem.text = attr
        
        # Pretty print
        rough_string = ET.tostring(root, encoding='unicode')
        reparsed = minidom.parseString(rough_string)
        return reparsed.toprettyxml(indent="    ")[23:]  # Remove XML declaration line


def main():
    parser = argparse.ArgumentParser(
        description="Convert between HDL and XML formats",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    format_converter.py input.hdl -o output.xml
    format_converter.py input.xml -o output.hdl
    format_converter.py protocols.hdl --output network.xml
        """
    )
    
    parser.add_argument(
        "input_file",
        help="Input file (.hdl or .xml)"
    )
    
    parser.add_argument(
        "-o", "--output",
        required=True,
        help="Output file (.hdl or .xml)"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    args = parser.parse_args()
    
    try:
        if args.verbose:
            print(f"Converting {args.input_file} -> {args.output}")
        
        converter = FormatConverter()
        converter.convert_file(args.input_file, args.output)
        
        if args.verbose:
            print("Conversion completed successfully")
        else:
            print(f"Successfully converted {args.input_file} -> {args.output}")
    
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
