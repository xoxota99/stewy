# Include Directory

This directory contains all header files for the Stewy project. Header files are organized into subdirectories based on their functionality:

- `core/`: Core functionality and common definitions
- `drivers/`: Hardware driver interfaces and implementations
- `ui/`: User interface related headers
- `platform/`: Platform-specific code and abstractions

## Usage Guidelines

1. Use `#include <stewy/...>` for project headers
2. Keep header files minimal and focused on a single responsibility
3. Use forward declarations when possible to minimize dependencies
4. Document all public interfaces with Doxygen-style comments
