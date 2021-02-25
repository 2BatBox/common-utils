#FAQ

##What is this?
'Common-utils' is a set of header-only libraries which are shared across github.com/2SilentJay projects.
Some of these projects use 'common-utils' as a git submodule.
The source code base is divided into three sections;

- **samples** - This part contains sample applications and any other experimental stuff.
It's the only compilable part of the project. The rest of it is a header-only code base.
**samples** depends on **lib**.
- **lib** - The library set itself. It has many sub-libraries inside. Some of them depend on each other.
- **test** - A unit test set which cover the core libraries components.

##How can I build and run the sample applications?
```
# Install dependencies first.
sudo apt-get install libpcap-dev

# Clone the project.
git clone https://github.com/2SilentJay/common-utils.git
cd common-utils

# Build it.
mkdir build
cd build
cmake ../
make
```
