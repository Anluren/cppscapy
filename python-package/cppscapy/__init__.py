"""
CPPScapy HDL Compiler Package

A comprehensive toolkit for compiling Hardware Description Language (HDL) and XML
protocol definitions into C++ header files for network packet manipulation.

This package provides:
- HDL and XML parsers for network protocol definitions
- C++ code generation for protocol headers
- Bidirectional format conversion between HDL and XML
- Schema validation for XML protocol definitions
- Command-line tools for compilation and conversion

Example usage:
    from cppscapy import HDLParser, XMLParser, CPPCodeGenerator

    # Parse HDL file
    parser = HDLParser('protocols.hdl')
    headers = parser.parse()

    # Generate C++ code
    generator = CPPCodeGenerator(headers)
    cpp_code = generator.generate()

Command-line tools:
    hdl-compile input.hdl -o output.h
    hdl-convert input.hdl -o output.xml
"""

__version__ = "1.0.0"
__author__ = "Dengfeng Zheng"
__email__ = "zhengdengfeng@gmail.com"
__license__ = "MIT"

from .hdl_compiler import HDLParser, XMLParser, UniversalParser, CPPCodeGenerator
from .format_converter import FormatConverter

__all__ = [
    "HDLParser",
    "XMLParser",
    "UniversalParser",
    "CPPCodeGenerator",
    "FormatConverter",
]
