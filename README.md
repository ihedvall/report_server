# Report Server Project

## Summary

The goal of the project is to simplify the storage and viewing of measurements. Measurements are stored in measurement 
files. This project choose to work with two types of files. The ASAM MDF4 is the best file for long-term storage while
the HDF5 files is the most used file format.

The project is a rather complex consisting of several libraries and applications.
- Library Util. It contains common utilities as logger.
- Library MDF. Interface for the MDF4 and MDF3 files writing and reading.
- Library ODS. Interface for the ASAM ODS database model. 
- Application MDF Viewer. The application is a simple MDF file viewer.
- Application ODS Modeler. The application configure ASAM ODS database models.
- Application Measurement Manager. The application manage multiple measurement files.

<<<<<<< HEAD
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
- wxWidgets Library. Is required for the GUI applications to build. Set both 
the 'wxWidgets_ROOT_DIR' and the 'wxWidgets_LIB_DIR' variables.

## License

The project uses the MIT license. See external LICENSE file in project root.
=======
The project is currently under development.
>>>>>>> b0fb5db134066228124ebd128d6b768f26188a72
