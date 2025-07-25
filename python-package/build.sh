#!/bin/bash
# CPPScapy HDL Compiler Package Build Script

set -e

echo "CPPScapy HDL Compiler Package Builder"
echo "====================================="

# Navigate to the package directory
cd "$(dirname "$0")"

# Clean previous builds
echo "Cleaning previous builds..."
rm -rf build/ dist/ *.egg-info/
find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
find . -name "*.pyc" -delete 2>/dev/null || true

# Install build dependencies
echo "Installing build dependencies..."
python -m pip install --upgrade pip setuptools wheel twine

# Build the package
echo "Building source distribution..."
python setup.py sdist

echo "Building wheel distribution..." 
python setup.py bdist_wheel

# Check the distributions
echo "Checking distributions..."
twine check dist/*

echo ""
echo "Build complete! Generated files:"
ls -la dist/

echo ""
echo "To install locally:"
echo "  pip install -e ."
echo ""
echo "To upload to PyPI (test):"
echo "  twine upload --repository-url https://test.pypi.org/legacy/ dist/*"
echo ""
echo "To upload to PyPI (production):"
echo "  twine upload dist/*"
