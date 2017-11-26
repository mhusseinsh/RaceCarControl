# TODO: make more generic

CXX = g++ -std=c++11
BIN = ./bin/
SRC = ./src/
INC = ./include/
PARAMS = -O3 `pkg-config --cflags --libs opencv`

all: VisionMain

clean: 
	rm -f $(BIN)*
	
ControlMain: $(BIN)Common.o $(BIN)ControlMain.o $(BIN)PDControl.o $(BIN)RCdriver.o $(BIN)TrackParser.o $(BIN)UDPclient.o $(BIN)UDPserver.o
	$(CXX) -O3 -o $(BIN)ControlMain $(BIN)ControlMain.o $(BIN)Common.o $(BIN)PDControl.o $(BIN)RCdriver.o $(BIN)TrackParser.o $(BIN)UDPclient.o $(BIN)UDPserver.o -lcomedi $(PARAMS)

DisplayMain: $(BIN)Common.o $(BIN)DisplayMain.o $(BIN)UDPserver.o 
	$(CXX) -O3 -o DisplayMain $(BIN)Common.o $(BIN)DisplayMain.o $(BIN)UDPserver.o $(PARAMS)

#VisionMain: $(BIN)Common.o $(BIN)VisionMain.o $(BIN)CameraHandler.o $(BIN)CameraCalibration.o $(BIN)Car.o $(BIN)ColorFilter.o $(BIN)KFilter.o $(BIN)PoseDetector.o $(BIN)UDPclient.o $(BIN)TrackParser.o 
#	$(CXX) -g -o VisionMain $(BIN)Common.o $(BIN)VisionMain.o $(BIN)CameraHandler.o $(BIN)CameraCalibration.o $(BIN)Car.o $(BIN)ColorFilter.o $(BIN)KFilter.o $(BIN)PoseDetector.o $(BIN)UDPclient.o $(BIN)TrackParser.o $(PARAMS)

#BlaMain: $(BIN)Common.o $(BIN)BlaMain.o $(BIN)Box.o $(BIN)CameraHandler.o $(BIN)CameraCalibration.o $(BIN)Car.o $(BIN)CarHandler.o $(BIN)ColorFilter.o $(BIN)ImageHandler.o $(BIN)KFilter.o $(BIN)PoseDetector.o $(BIN)UDPclient.o $(BIN)TrackParser.o 
#	$(CXX) -g -o BlaMain $(BIN)Common.o $(BIN)BlaMain.o $(BIN)Box.o $(BIN)CameraHandler.o $(BIN)CameraCalibration.o $(BIN)Car.o $(BIN)CarHandler.o $(BIN)ColorFilter.o $(BIN)ImageHandler.o $(BIN)KFilter.o $(BIN)PoseDetector.o $(BIN)UDPclient.o $(BIN)TrackParser.o $(PARAMS)

VisionMain: $(BIN)Common.o $(BIN)VisionMain.o $(BIN)Box.o $(BIN)CameraHandler.o $(BIN)CameraCalibration.o $(BIN)Car.o $(BIN)CarHandler.o $(BIN)ColorFilter.o $(BIN)ImageHandler.o $(BIN)KFilter.o $(BIN)PoseDetector.o $(BIN)UDPclient.o $(BIN)TrackParser.o 
	$(CXX) -O3 -o $(BIN)VisionMain $(BIN)Common.o $(BIN)VisionMain.o $(BIN)Box.o $(BIN)CameraHandler.o $(BIN)CameraCalibration.o $(BIN)Car.o $(BIN)CarHandler.o $(BIN)ColorFilter.o $(BIN)ImageHandler.o $(BIN)KFilter.o $(BIN)PoseDetector.o $(BIN)UDPclient.o $(BIN)TrackParser.o $(PARAMS)

$(BIN)%.o: $(SRC)%.cpp
	$(CXX) -O3 -o $@ -c $<
