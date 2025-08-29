#!/usr/bin/env python3
"""
CPPScapy HDL Compiler - Setup Script

A Python package for compiling Hardware Description Language (HDL) and XML
protocol definitions into C++ header files for network packet manipulation.
"""

from setuptools import setup, find_packages
import os


# Read the README file for long description
def read_readme():
    readme_path = os.path.join(os.path.dirname(__file__), "README.md")
    if os.path.exists(readme_path):
        with open(readme_path, "r", encoding="utf-8") as f:
            return f.read()
    return "CPPScapy HDL Compiler - Network Protocol Header Generator"


# Read requirements
def read_requirements():
    req_path = os.path.join(os.path.dirname(__file__), "requirements.txt")
    if os.path.exists(req_path):
        with open(req_path, "r", encoding="utf-8") as f:
            return [
                line.strip() for line in f if line.strip() and not line.startswith("#")
            ]
    return []


setup(
    name="cppscapy-hdl-compiler",
    version="1.0.0",
    author="Dengfeng Zheng",
    author_email="zhengdengfeng@gmail.com",
    description="HDL and XML to C++ network protocol header compiler",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    url="https://github.com/Anluren/cppscapy",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Software Development :: Code Generators",
        "Topic :: System :: Networking",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.7",
    install_requires=read_requirements(),
    extras_require={
        "xml": ["lxml>=4.6.0"],
        "dev": ["lxml>=4.6.0", "black", "flake8", "pytest"],
        "all": ["lxml>=4.6.0", "black", "flake8", "pytest"],
    },
    entry_points={
        "console_scripts": [
            "hdl-compile=cppscapy.hdl_compiler:main",
            "hdl-convert=cppscapy.format_converter:main",
        ],
    },
    package_data={
        "cppscapy": [
            "examples/*.hdl",
            "examples/*.xml",
            "examples/*.xsd",
            "docs/*.md",
        ],
    },
    include_package_data=True,
    zip_safe=False,
    keywords="hdl xml cpp network protocol packet header compiler scapy",
    project_urls={
        "Bug Reports": "https://github.com/Anluren/cppscapy/issues",
        "Source": "https://github.com/Anluren/cppscapy",
        "Documentation": ("https://github.com/Anluren/cppscapy/blob/main/README.md"),
    },
)
