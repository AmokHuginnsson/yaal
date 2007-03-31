CXX ?= g++
CXX := $(subst -x c++ -x c++,-x c++,$(subst colorgcc,colorgcc -x c++,$(CXX)))
DXX ?= $(CXX)
DXX := $(subst -x c++,,$(subst colorg,g,$(DXX)))
LXX ?= g++
