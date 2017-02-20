
int MaxVoltage; //overvoltage value
int MaxCurrent; //overcurrent value
int MinVoltage; //undervoltage value

int ActiveLoad[10]; //list of all active loads
int InactiveLoad[10]; //list of all inactive loads


float cVoltage; // voltage from sensor
float cCurrent; // current from sensor

char __loadProfile[] = "{'fan':1,'socket':2,'light':3}";

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}


float getVoltage(){
  //placeholder
  //get voltage from voltage sensor
  }

  
float getCurrent(){
  //placeholder
  //get current from current sensor
  }

 void safetyCheck(){
   // **critical function**
   // anonymous function
   //check to see if values are within a safe range
   cCurrent = getCurrent();
   cVoltage = getVoltage();

   if (cCurrent >= MaxCurrent
   || cVoltage <= MinVoltage
   || cVoltage >= MaxVoltage){
    
    //cutoff power
    cutoffPower();
   }
  
 }


bool cutoffPower(){
  //**critical function**
  //trigger relay to disconnect source
}


bool powerOnLoad(char id){
  //power on the load by its specified id
  //generic function

}

bool powerOffLoad(int id){
  //power off the load by its specified id
  //generic function

}

void syncLoads(){
   // **critical function**
  //synchronize active and inactive loads
  for(int i=0; i<sizeof(ActiveLoad)-1;i++){
    if (powerOnLoad(ActiveLoad[i]) != true){
      powerOffLoad(ActiveLoad[i]);
    }
  }

  for(int i=0;i<sizeof(InactiveLoad)-1;i++){
    if (powerOffLoad(InactiveLoad[i]) != true){
      powerOnLoad(InactiveLoad[i]);
    }
  }
}


void hubSync(){
  //synchronize hub with server
  getServerStat();
  pushLocalStat();
  syncLoads();
}

void getServerStat(){
  //get details from server (json format)
  char data = __get(); //pull from server
//  cVoltage = data['voltage']
//  cCurrent = data['current']
//  ActiveLoad = data['aload']
//  InActiveLoad = data['iload']
  
}

void pushLocalStat(){
  // push local storage value to server (json format)
  cVoltage = getVoltage();
  cCurrent = getCurrent();

//  char data = {'voltage':voltage,'current': current,'aload': ActiveLoad,'iload': InactiveLoad};
 
    __post(); //push to server
}
char __get(){
  //helper function for get
}

char __post(){
  //helper function for post
}

