"""
Test suite for CPPScapy HDL Compiler package
"""

import unittest
import tempfile
import os
import sys
from pathlib import Path

# Add the package to the path for testing
package_dir = Path(__file__).parent.parent
sys.path.insert(0, str(package_dir))

from cppscapy import HDLParser, XMLParser, CPPCodeGenerator, FormatConverter


class TestHDLCompiler(unittest.TestCase):
    """Test HDL compiler functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.test_hdl = '''
enum TestEnum : uint8_t {
    VALUE1 = 1;
    VALUE2 = 2;
}

header TestHeader {
    field1: 8;
    field2: 16 = 0x1234;
    field3: TestEnum;
}
'''
        self.test_xml = '''<?xml version="1.0" encoding="UTF-8"?>
<network_protocols>
    <enum name="TestEnum" underlying_type="uint8_t">
        <value name="VALUE1" value="1"/>
        <value name="VALUE2" value="2"/>
    </enum>
    <header name="TestHeader" description="Test header">
        <field name="field1" bit_width="8" type="integer" description="Field 1"/>
        <field name="field2" bit_width="16" type="integer" default="0x1234" description="Field 2"/>
        <field name="field3" bit_width="8" type="enum" enum_type="TestEnum" description="Field 3"/>
    </header>
</network_protocols>
'''

    def test_hdl_parser(self):
        """Test HDL parsing"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.hdl', delete=False) as f:
            f.write(self.test_hdl)
            f.flush()
            
            parser = HDLParser(f.name)
            headers, enums = parser.parse()
            
            self.assertEqual(len(headers), 1)
            self.assertEqual(len(enums), 1)
            self.assertEqual(headers[0]['name'], 'TestHeader')
            self.assertEqual(enums[0]['name'], 'TestEnum')
            
            os.unlink(f.name)

    def test_xml_parser(self):
        """Test XML parsing"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.xml', delete=False) as f:
            f.write(self.test_xml)
            f.flush()
            
            parser = XMLParser(f.name)
            headers, enums = parser.parse()
            
            self.assertEqual(len(headers), 1)
            self.assertEqual(len(enums), 1)
            self.assertEqual(headers[0]['name'], 'TestHeader')
            self.assertEqual(enums[0]['name'], 'TestEnum')
            
            os.unlink(f.name)

    def test_cpp_generation(self):
        """Test C++ code generation"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.hdl', delete=False) as f:
            f.write(self.test_hdl)
            f.flush()
            
            parser = HDLParser(f.name)
            headers, enums = parser.parse()
            
            generator = CPPCodeGenerator(headers, enums)
            cpp_code = generator.generate()
            
            self.assertIn('struct TestHeader', cpp_code)
            self.assertIn('enum class TestEnum', cpp_code)
            self.assertIn('uint8_t field1 : 8', cpp_code)
            
            os.unlink(f.name)

    def test_format_conversion(self):
        """Test format conversion"""
        converter = FormatConverter()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.hdl', delete=False) as hdl_file:
            hdl_file.write(self.test_hdl)
            hdl_file.flush()
            
            with tempfile.NamedTemporaryFile(mode='w', suffix='.xml', delete=False) as xml_file:
                # Test HDL to XML conversion
                converter.hdl_to_xml(hdl_file.name, xml_file.name)
                
                # Verify the XML file was created and has content
                self.assertTrue(os.path.exists(xml_file.name))
                with open(xml_file.name, 'r') as f:
                    xml_content = f.read()
                    self.assertIn('<header name="TestHeader"', xml_content)
                    self.assertIn('<enum name="TestEnum"', xml_content)
                
                os.unlink(hdl_file.name)
                os.unlink(xml_file.name)


if __name__ == '__main__':
    unittest.main()
