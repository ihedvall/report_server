# Report Server Project

## Summary

The goal of the project is to simplify the storage and viewing of measurements. Measurements are stored in measurement 
files. This project choose to work with the ASAM MDF data files. This format is the best file for 
long-term storage.

The project only includes applications (executable). The applications uses 4 main libraries which are stored
in their own repositories.

- UtilLib. This is a general purpose library used by all applications.
- MDFLib. This library include all MDF related interfaces.
- ODSLib- This library include all ODS related interfaces.
- PubSubLib. This library include publish/subscribe interface as MQTT.

The project is currently under development.

Most applications has been moved to the above libraries.

The project specification is in [Functional Specification](specification/report_server.docx)

## Building the project

To be defined.


## License

The project uses the MIT license. See external LICENSE file in project root.


