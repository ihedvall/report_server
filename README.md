# Report Server Project

## Summary

The goal of the project is to simplify the storage and viewing of measurements. Measurements are stored in measurement 
files. This project choose to work with the ASAM MDF data files. This format is the best file for 
long-term storage.

The project only includes applications (executable). The applications use three main libraries which are stored
in their own repositories.

- UtilLib. This is a general purpose library used by all applications.
- MDFLib. This library include all MDF related interfaces.
- ODSLib- This library include all ODS related interfaces.

The project consisting of several  applications.
- Service Daemon. Non-GUI MS Window service that supervises other non-service executable.
- Service Explorer. GUI application that mainly is used for configure the service daemons.
- Listen Daemon. Non-GUI application that maintains all listen servers on one computer.
- Listen Viewer. GUI application that displays listen (trace) messages. 
- MDF Viewer. GUI application that show information about a MDF files.
- ODS Configurator. GUI application that configure ASAM ODS database models.
- Report Explorer. GUI stand-alone application that maintains and creates reports for up to 10K measurement files.
- Report Server. Server with webb-based GUI that maintains and schedule report for a large number of tests and measurements.
The project is currently under development.

## Building the project

The project uses CMAKE for building. The following third-party libraries are used and
needs to be downloaded and built.

- Boost Library. Set the 'Boost_ROOT' variable to the Boost root path.
- Expat Library. Set the 'EXPAT_ROOT' variable to the Expat root path.
- OpenSSL Library. Set the 'OPENSSL_ROOT' variable to the OpenSSL root path.
- ZLIB Library. Set the 'ZLIB_ROOT' variable to the ZLIB root path.
- Doxygen's application. Is required if the documentation should be built.
- Google Test Library. Is required for running and build the unit tests.
- wxWidgets Library. Is required for the GUI applications to build. Set both the 'wxWidgets_ROOT_DIR' and the 'wxWidgets_LIB_DIR' variables.
- NSIS Installer. Needed to build an installer executable for Windows.

## License

The project uses the MIT license. See external LICENSE file in project root.

The project is currently under development.

