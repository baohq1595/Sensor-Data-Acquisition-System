
public class Tag {
	
	//Package max size
	public final static int PACKAGE_MAX_SIZE = 45;
	
	//Client type tags
	public final static String HUB_SENSOR = "HUBSSR";
	public final static String PC_CLIENT = "PCCLNT";
	public final static String SERVER = "SERVER";
	
	//Action type tags
	public final static String NOTIFY = "NOTIFY";
	public final static String GET_DATA = "GETDAT";
	public final static String SEND_DATA = "SENDAT";
	public final static String HAND_SHAKING = "HANDSH";
	public final static String UPDATE_DATA = "UPDATA";
	public final static String READY = "READY_";
	public final static String END_OF_DATA = "ENDDAT";
	
	//Sensor type tags
	public final static String LIGHT_SENSOR = "LIGHT";
	public final static String TEMP_SENSOR = "TEMPA";
	public final static String HUMIDITY = "HUMID";
	
	//SensorHub name tags
	public final static String HUB01 = "HUB01";
	
	//Sensor ID tags
	public final static String SENSOR_TEMP_1 = "T001";
	public final static String SENSOR_LIGHT_1 = "L001";
	public final static String SENSOR_HUMIDITY_1 = "H001";
	
	//Size tags
	public final static int CLIENT_TYPE_SIZE = 6;
	public final static int CONTROL_SIGNAL_SIZE = 6;
	public final static int SENSOR_TYPE_SIZE = 5;
	public final static int SENSORHUB_NAME_SIZE = 5;
	public final static int SENSOR_ID_SIZE = 4;
	
	//State
	public final static String INIT = "START";
	public final static String IN_PROGRESS = "PROGRESS";
	public final static String WAITING = "WAITING";
	
	//signal
	public final static String READY_TO_SEND = "READY_TO_SEND";
	public final static String READY_TO_RECV = "READY_TO_RECV";
	public final static int SIGNAL_SIZE = 13;
}
