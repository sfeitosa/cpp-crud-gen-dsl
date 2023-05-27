# A JSON-like DSL to Generate Web CRUD Applications in C++

This repository contains the code used and described in the paper submitted for the 27th Brazilian Symposium on Programming Languages (2023).

## Usage

The next steps will show you how to compile and run an example application using the proposed DSL.

### Compiling and installing

To compile and install the software and libraries, run the following commands.

```
cmake .
make && make install
```

### Running the example

To execute the example application, run the following commands to start an HTTP server:

```bash
cd install
python3 -m http.server --cgi 8000 --directory var/www/html/
```

### Testing the example

Then, open your browser and access the page [http://localhost:8000/cgi-bin/swm.cgi](http://localhost/cgi-bin/swm.cgi).

The credentials to access the example page is ``admin/admin``.


