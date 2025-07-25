# HDL Compiler: XML vs HDL Format Support

The HDL Compiler now supports **both XML and HDL formats** for describing network protocol headers. This document explains the differences, advantages, and usage of each format.

## 🔄 Format Comparison

### HDL Format (Original)
```hdl
enum EtherType : uint16_t {
    IPv4 = 0x0800,
    IPv6 = 0x86DD,
    ARP = 0x0806
}

header EthernetHeader {
    dst_mac: 48;
    src_mac: 48;
    ethertype: EtherType;
}
```

### XML Format (New)
```xml
<enum name="EtherType" underlying_type="uint16_t">
    <value name="IPv4" value="0x0800"/>
    <value name="IPv6" value="0x86DD"/>
    <value name="ARP" value="0x0806"/>
</enum>

<header name="EthernetHeader">
    <description>Standard Ethernet frame header</description>
    <field name="dst_mac" bit_width="48" type="integer">
        <description>Destination MAC address</description>
    </field>
    <field name="src_mac" bit_width="48" type="integer">
        <description>Source MAC address</description>
    </field>
    <field name="ethertype" bit_width="16" type="enum" enum_type="EtherType">
        <description>EtherType indicating the protocol of the next layer</description>
    </field>
</header>
```

## ✨ XML Format Advantages

### 1. **Rich Metadata Support**
- **Descriptions**: Add documentation directly in the protocol definition
- **Attributes**: Specify field properties like `computed`, `readonly`
- **Extensibility**: Easy to add new metadata without breaking compatibility

### 2. **Schema Validation**
- **XSD Support**: Validate structure and data types before compilation
- **IDE Integration**: Syntax highlighting, auto-completion, error detection
- **Tool Ecosystem**: Works with existing XML tools and parsers

### 3. **Enhanced Expressiveness**
```xml
<field name="header_checksum" bit_width="16" type="integer">
    <description>Header checksum for error detection</description>
    <attributes>
        <attribute>computed</attribute>
    </attributes>
</field>
```

### 4. **Better Tooling**
- **XML Editors**: Professional XML IDEs with validation
- **Version Control**: Better diff/merge support in many tools
- **Processing**: Easy to transform with XSLT, parse with standard libraries

## 🚀 Usage Examples

### Automatic Format Detection
The compiler automatically detects the format based on file extension:

```bash
# XML format
python3 hdl_compiler.py protocols.xml -o output.h

# HDL format  
python3 hdl_compiler.py protocols.hdl -o output.h
```

### Content-Based Detection
If the extension is ambiguous, the compiler examines file content:

```bash
# Works regardless of extension
python3 hdl_compiler.py network_definitions.txt -o output.h
```

## 🔧 XML Schema Features

### Complete Field Definition
```xml
<field name="version" bit_width="4" type="integer" default="4">
    <description>IP version (always 4 for IPv4)</description>
    <attributes>
        <attribute>readonly</attribute>
    </attributes>
</field>
```

### Enum with Rich Metadata
```xml
<enum name="IPProtocol" underlying_type="uint8_t">
    <value name="ICMP" value="1"/>
    <value name="TCP" value="6"/>
    <value name="UDP" value="17"/>
</enum>
```

### Complex Headers
```xml
<header name="IPv4Header">
    <description>Internet Protocol version 4 header</description>
    <!-- Multiple fields with various types -->
    <field name="version" bit_width="4" type="integer" default="4"/>
    <field name="protocol" bit_width="8" type="enum" enum_type="IPProtocol"/>
    <field name="checksum" bit_width="16" type="integer">
        <attributes>
            <attribute>computed</attribute>
        </attributes>
    </field>
</header>
```

## 🛠️ Development Workflow

### 1. **Create Protocol Definition**
Choose your preferred format:
- `protocols.xml` - For rich documentation and tooling
- `protocols.hdl` - For concise, code-like syntax

### 2. **Validate (XML only)**
```bash
xmllint --schema network_protocols.xsd protocols.xml --noout
```

### 3. **Compile to C++**
```bash
python3 hdl_compiler.py protocols.xml -o generated_headers.h --verbose
```

### 4. **Use in C++ Code**
```cpp
#include "generated_headers.h"
using namespace cppscapy::dsl;

EthernetHeader eth;
eth.set_dst_mac(0x001122334455);
eth.set_ethertype(EtherType::IPv4);
```

## 🔍 VS Code Integration

The updated `.vscode` configuration supports both formats:

### File Associations
- `*.hdl` → Plain text (syntax highlighting for HDL)
- `*.xml` → XML (rich XML support)
- `*.xsd` → XML Schema

### Debug Configurations
- **"Debug HDL Compiler"** - Test with XML format
- **"Debug HDL Compiler (HDL Format)"** - Test with HDL format
- **"Debug HDL Compiler (Custom Input)"** - Test with any format

### Tasks
- **"Run HDL Compiler"** - Compile XML by default
- **"Run HDL Compiler (HDL Format)"** - Compile HDL format
- **"Run HDL Compiler (Custom)"** - Prompt for input file

## 📊 Format Comparison Matrix

| Feature | HDL Format | XML Format |
|---------|------------|------------|
| **Conciseness** | ✅ Very concise | ❌ More verbose |
| **Readability** | ✅ Code-like syntax | ✅ Clear structure |
| **Documentation** | ❌ Comments only | ✅ Embedded descriptions |
| **Schema Validation** | ❌ No standard | ✅ XSD validation |
| **IDE Support** | ❌ Basic | ✅ Excellent |
| **Tool Ecosystem** | ❌ Custom only | ✅ Rich ecosystem |
| **Extensibility** | ❌ Syntax changes needed | ✅ Easy extension |
| **Learning Curve** | ✅ Simple | ❌ XML knowledge needed |

## 🎯 Recommendations

### Use **HDL Format** when:
- You prefer concise, code-like syntax
- Working with simple protocol definitions
- Quick prototyping and experimentation
- Team is familiar with domain-specific languages

### Use **XML Format** when:
- You need rich documentation and metadata
- Working in enterprise environments with tooling requirements
- Protocol definitions are complex or need validation
- Integration with XML-based workflows
- Long-term maintenance and documentation is important

## 🔄 Migration Path

### HDL to XML
Use the compiler's auto-detection to gradually migrate:

1. **Keep existing HDL files** - They continue to work
2. **Create new definitions in XML** - For enhanced features
3. **Convert critical protocols** - When documentation is needed
4. **Maintain both formats** - No need to convert everything

### Backward Compatibility
The compiler maintains 100% backward compatibility with existing HDL files while adding XML support as an enhancement.
