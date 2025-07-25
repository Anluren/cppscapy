#!/bin/bash
# CPPScapy HDL Compiler Development Setup Script

set -e

echo "CPPScapy HDL Compiler Development Setup"
echo "======================================="

# Navigate to the package directory
cd "$(dirname "$0")"

# Install in development mode
echo "Installing package in development mode..."
python -m pip install -e .

# Install development dependencies
echo "Installing development dependencies..."
python -m pip install pytest black flake8 mypy

# Run tests
echo "Running tests..."
python -m pytest tests/ -v

# Run linting
echo "Running code quality checks..."
echo "  - Black formatting check..."
if black --check --diff cppscapy/; then
    echo "    ✅ Code is properly formatted"
else
    echo "    ⚠️  Code formatting issues found"
    read -p "    Would you like to auto-format the code? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "    Auto-formatting code..."
        black cppscapy/
        echo "    ✅ Code formatted successfully"
    fi
fi

echo "  - Flake8 linting..."
flake8 cppscapy/ --max-line-length=88 --extend-ignore=E203,W503

echo ""
echo "Development setup complete!"
echo ""
echo "Available commands:"
echo "  hdl-compile - Compile HDL/XML to C++ headers"
echo "  hdl-convert - Convert between HDL and XML formats"
echo ""
echo "Example usage:"
echo "  hdl-compile examples/network_protocols.xml -o output.h"
echo "  hdl-convert examples/network_protocols.xml -o output.hdl"
