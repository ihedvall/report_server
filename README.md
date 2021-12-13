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

The project is currently under development.