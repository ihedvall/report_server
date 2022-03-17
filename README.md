# Report Server Project

## Summary

The goal of the project is to simplify the storage and viewing of measurements. Measurements are stored in measurement 
files. This project choose to work with two types of files. The ASAM MDF4 is the best file for long-term storage while
the HDF5 files is the most used file format.

The project is a rather complex consisting of several libraries and applications.
- Library Util. It contains common utilities as logger.
- Library MDF. Interface for the MDF4 and MDF3 files writing and reading.
- Library ODS. Interface for the ASAM ODS database model. 
- Application Service Daemon. Non-GUI MS Window service that supervises other non-service executable.
- Application Service Explorer. GUI application that mainly is used for configure the service daemons.
- Application Listen Daemon. Non-GUI application that maintains all listen servers on one computer.
- Application Listen Viewer. GUI application that displays listen (trace) messages. 
- Application MDF Viewer. GUI application that show information about a MDF files.
- Application ODS Configurator. GUI application that configure ASAM ODS database models.
- Report Explorer. GUI stand-alone application that maintains and creates reports for up to 10K measurement files.
- Report Server. Server with webb-based GUI that maintains and schedule report for a large number of tests and measurements.
The project is currently under development.

## Building the project

The project uses CMAKE for building. The following third-party libraries are used and
needs to be downloaded and built.

- Boost Library. Set the 'Boost_ROOT' variable to the Boost root path.
- Expat Library. Set the 'EXPAT_ROOT' variable to the expat root path.
- OpenSSL Library. Set the 'OPENSSL_ROOT' variable to the OpenSSL root path.
- ZLIB Library. Set the 'ZLIB_ROOT' variable to the ZLIB root path.
- Doxygen's application. Is required if the documentation should be built.
- Google Test Library. Is required for running and build the unit tests.
- wxWidgets Library. Is required for the GUI applications to build. Set both the 'wxWidgets_ROOT_DIR' and the 'wxWidgets_LIB_DIR' variables.
- NSIS Installer. Needed to build an installer executable for Windows.


## License

The project uses the MIT license. See external LICENSE file in project root.

The project is currently under development.

