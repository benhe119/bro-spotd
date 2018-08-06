### bro_spotd makefile
#
# @author = asr

### INSTALL DIRECTORIES
DESTDIR =
###

VERSION = 1.0

# Current directory
CURDIR = $(realpath .)

# folder of the headers
INC_DIR = $(CURDIR)/include

# folder of the sources
SRC_DIR = $(CURDIR)/src

#folder of the built sources
OBJ_DIR = $(CURDIR)/build

#folder of the exec files
BIN_DIR = $(CURDIR)/bin

#folder for the bro script
TEST_DIR = $(CURDIR)/test

#folder for the services
BRO_DIR = $(CURDIR)/bro

#folder for the services
SERVICE_DIR = $(CURDIR)/service

#folder of the xml interface
INTERFACE_DIR = $(CURDIR)/interface

# install folders
INSTALL_BIN_DIR = $(DESTDIR)/usr/bin
INSTALL_CONF_DIR = $(DESTDIR)/etc/bro-spotd
INSTALL_SYSTEMD_SERVICE_DIR = $(DESTDIR)/etc/systemd/user
INSTALL_DBUS_SERVICE_DIR = $(DESTDIR)/usr/share/dbus-1/services
INSTALL_DBUS_INTERFACE_DIR = $(DESTDIR)/usr/share/dbus-1/interfaces
INSTALL_BRO_SCRIPT_DIR = $(DESTDIR)/usr/share/bro/policy/custom
BRO_SCRIPT_LOAD_DIR = $(DESTDIR)/etc/bro/site

# all the files (header, sources, build)
FILES = BroConnection.h MonitoredFeature.h Dispotcher.h Dbuspotcher.h
DEPS = $(foreach n,$(FILES),$(INC_DIR)/$(n))
SRCS = $(foreach n,$(FILES:.h=.cpp),$(SRC_DIR)/$(n)) $(SRC_DIR)/main.cpp
OBJS = $(FILES:.h=.o) main.o
# library file
TARGET = bro-spotd

# compilo
CXX = @g++
CXXFLAGS = -std=c++11 -Wall -pedantic -g -Wl,--no-whole-archive
INC = -I/usr/include/iniparser -I/usr/include/libspot -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -Iinclude
LIB = -L/usr/lib/x86_64-linux-gnu/
LDD = -lbroccoli -lspot -liniparser -lpthread -ldbus-1

### MAKEFILE TARGETS
all: $(checkdir) $(TARGET)

checkdir:
	@echo
	@echo "==== bro-spotd" $(VERSION) "===="
	@echo
	@echo "Checking the exec directory ("$(BIN_DIR)")"
	@mkdir -p $(LIB_DIR)
	@echo "Checking the object directory ("$(OBJ_DIR)")"
	@mkdir -p $(OBJ_DIR)
	@echo
	@echo "[Building sources]"

%.o:$(SRC_DIR)/%.cpp
	@echo "Building" $@ "..."
	$(CXX) $(CXXFLAGS) $(INC) $(LIB) -c $< -o $(OBJ_DIR)/$@ $(LDD);

$(TARGET): $(OBJS)
	@echo
	@echo "[Building excutable]"
	@echo "Building" $@ "..."
	$(CXX) $(CXXFLAGS) $(INC) $(LIB) $(foreach n,$^,$(OBJ_DIR)/$(n)) -o $(BIN_DIR)/$@ $(LDD);
	@echo "[done]"

install: install_bin install_conf install_services install_interface install_bro_script

install_bin:
	@echo
	@echo "Installing executable"
	@echo $(TARGET) "->" $(INSTALL_BIN_DIR)
	mkdir -p $(INSTALL_BIN_DIR)
	@install $(BIN_DIR)/$(TARGET) $(INSTALL_BIN_DIR)

install_conf:
	@echo
	@echo "Installing configuration"
	@echo $(BIN_DIR)/bro-spotd.ini "->" $(INSTALL_CONF_DIR)
	mkdir -p $(INSTALL_CONF_DIR)
	@install $(BIN_DIR)/bro-spotd.ini $(INSTALL_CONF_DIR)

install_services:
	@echo
	@echo "Installing services"
	@echo $(SERVICE_DIR)/dbus-bro.spot.service "->" $(INSTALL_SYSTEMD_SERVICE_DIR)
	mkdir -p $(INSTALL_SYSTEMD_SERVICE_DIR)
	@install $(SERVICE_DIR)/dbus-bro.spot.service $(INSTALL_SYSTEMD_SERVICE_DIR)
	@echo $(SERVICE_DIR)/bro.spot.service "->" $(INSTALL_DBUS_SERVICE_DIR)
	mkdir -p $(INSTALL_DBUS_SERVICE_DIR)
	@install $(SERVICE_DIR)/bro.spot.service $(INSTALL_DBUS_SERVICE_DIR)

install_interface:
	@echo
	@echo "Installing D-BUS interface"
	@echo $(INTERFACE_DIR)/bro.spot.xml "->" $(INSTALL_DBUS_INTERFACE_DIR)
	@mkdir -p $(INSTALL_DBUS_INTERFACE_DIR)
	@install $(INTERFACE_DIR)/bro.spot.xml $(INSTALL_DBUS_INTERFACE_DIR)

install_bro_script:
	@echo
	@echo "Installing Bro Script"
	@echo $(BRO_DIR)/spot-anomaly.bro "->" $(INSTALL_BRO_SCRIPT_DIR)
	@mkdir -p $(INSTALL_BRO_SCRIPT_DIR)
	@mkdir -p $(BRO_SCRIPT_LOAD_DIR)
	@install $(BRO_DIR)/spot-anomaly.bro $(INSTALL_BRO_SCRIPT_DIR)
	@echo "Updating " $(BRO_SCRIPT_LOAD_DIR)/local.bro
	@echo "\n# Bro-Spot script (log spot anomalies)\n@load custom/spot-anomaly\n" >> $(BRO_SCRIPT_LOAD_DIR)/local.bro



