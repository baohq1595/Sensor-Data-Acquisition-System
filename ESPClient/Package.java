import java.io.UnsupportedEncodingException;
import java.util.Arrays;

public class Package {
	
	private String clientType;
	
	private int yy;
	private int mm;
	private int dd;
	private int hour;
	private int minute;
	private int second;
	
	private String SensorHubName;
	private String SensorID;
	private String SensorType;
	private int value;
	
	private String control;
	
	
	public Package()
	{
		clientType = "PCCLNT";
		SensorHubName = "NONE";
		SensorID = "X000";
		SensorType = "X";
	}
	
	public Package(String clienType, String control,
			int yy, int mm, int dd, int hour, int minute, int second,
			String sshn, String ssid, String sst,
			int value)
	{
		this.clientType = clienType;
		this.control = control;
		this.yy = yy;
		this.mm = mm;
		this.dd = dd;
		this.hour = hour;
		this.minute = minute;
		this.second = second;
		this.SensorHubName = sshn;
		this.SensorID = ssid;
		this.SensorType = sst;
		this.value = value;
	}
	
	public void setClientType(String clntType)
	{
		this.clientType = clntType;
	}
	
	public void setControl(String ctrl)
	{
		this.control = ctrl;
	}
	
	public void setYY(int yy)
	{
		this.yy = yy;
	}
	
	public void setMM(int mm)
	{
		this.mm = mm;
	}
	
	public void setDD(int dd)
	{
		this.dd = dd;
	}
	
	public void setHour(int hour)
	{
		this.hour = hour;
	}
	
	public void setMinute(int minute)
	{
		this.minute = minute;
	}
	
	public void setSecond(int second)
	{
		this.second = second;
	}
	
	public void setSensorHubName(String name)
	{
		this.SensorHubName = name;
	}
	
	public void setSensorID(String id)
	{
		this.SensorID = id;
	}
	
	public void setSensorType(String type)
	{
		this.SensorType = type;
	}
	
	public void setValue(int value)
	{
		this.value = value;
	}
	
	public String getClientType()
	{
		return this.clientType;
	}
	
	public String getControl()
	{
		return this.control;
	}
	
	public int getYY()
	{
		return this.yy;
	}
	
	public int getMM()
	{
		return this.mm;
	}
	
	public int getDD()
	{
		return this.dd;
	}
	
	public int getHour()
	{
		return this.hour;
	}
	
	public int getMinute()
	{
		return this.minute;
	}
	
	public int getSecond()
	{
		return this.second;
	}
	
	public String getTime()
	{
		return String.format("%d/%d/%d %d:%d:%d", this.yy, this.mm, this.dd, this.hour, this.minute, this.second);
	}
	
	public String getSensorHubName()
	{
		return this.SensorHubName;
	}
	
	public String getSensorID()
	{
		return this.SensorID;
	}
	
	public String getSensorType()
	{
		return this.SensorType;
	}
	
	public int getValue()
	{
		return this.value;
	}
	
	public void Serialize(byte[] buffer)
	{
		byte[] byteBuffer = new byte[10];
		byte []floatBuffer = new byte[8];
		byte[] intBuffer = new byte[4];
		int i = 0;
		
		byteBuffer = clientType.getBytes();	
		System.arraycopy(byteBuffer, 0, buffer, i, Tag.CLIENT_TYPE_SIZE);
		i = i + Tag.CLIENT_TYPE_SIZE;
		
		byteBuffer = control.getBytes();	
		System.arraycopy(byteBuffer, 0, buffer, i, Tag.CONTROL_SIGNAL_SIZE);
		i = i + Tag.CONTROL_SIGNAL_SIZE;
		
//		ByteBuffer.wrap(intBuffer).putInt(yy);
		String year = Integer.toString(yy);
		String temp1 = year;
		for (int j = 0; j < 4 - temp1.length(); j++)
		{
			year = new String("0" + year);
		}
		System.arraycopy(year.getBytes(), 0, buffer, i, 4);
		i = i + 4;
		
//		ByteBuffer.wrap(intBuffer).putInt(mm);
		String month = Integer.toString(mm);
		if(month.length() != 2) month =  new String("0" + month);
		System.arraycopy(month.getBytes(), 0, buffer, i, 2);
		i = i + 2;
		
//		ByteBuffer.wrap(intBuffer).putInt(dd);
		String day = Integer.toString(dd);
		if(day.length() != 2) day = new String("0" + day);
		System.arraycopy(day.getBytes(), 0, buffer, i, 2);
		i = i + 2;
		
//		ByteBuffer.wrap(intBuffer).putInt(hour);
		String s_hour = Integer.toString(hour);
		if(s_hour.length() != 2) s_hour = new String("0" + s_hour);
		System.arraycopy(s_hour.getBytes(), 0, buffer, i, 2);
		i = i + 2;
		
//		ByteBuffer.wrap(intBuffer).putInt(minute);
		String s_minute = Integer.toString(minute);
		if(s_minute.length() != 2) s_minute = new String("0" + s_minute);
		System.arraycopy(s_minute.getBytes(), 0, buffer, i, 2);
		i = i + 2;
		
//		ByteBuffer.wrap(intBuffer).putInt(second);
		String s_second = Integer.toString(second);
		if(s_second.length() != 2) s_second = new String("0" + s_second);
		System.arraycopy(s_second.getBytes(), 0, buffer, i, 2);
		i = i + 2;
		
		byteBuffer = SensorHubName.getBytes();
		
		System.arraycopy(byteBuffer, 0, buffer, i, Tag.SENSORHUB_NAME_SIZE);
		i = i + Tag.SENSORHUB_NAME_SIZE;
		
		byteBuffer = SensorID.getBytes();
		
		System.arraycopy(byteBuffer, 0, buffer, i, Tag.SENSOR_ID_SIZE);
		i = i + Tag.SENSOR_ID_SIZE;
		
		byteBuffer = SensorType.getBytes();
		
		System.arraycopy(byteBuffer, 0, buffer, i, Tag.SENSOR_TYPE_SIZE);
		i = i + Tag.SENSOR_TYPE_SIZE;
		
//		ByteBuffer.wrap(floatBuffer).putFloat(value);
		String s_value = Integer.toString(value);
		String temp2 = s_value;
		for (int j = 0; j < 5 - temp2.length(); j++)
		{
			s_value = new String("0" + s_value);
		}
		System.arraycopy(s_value.getBytes(), 0, buffer, i, 5);
	}
	
	public void Deserialize(byte[] buffer)
	{
		int i = 0;
		byte [] strBuffer = new byte[10];
		byte [] intBuffer = new byte[4];
		byte [] floatBuffer = new byte[4];
		String temp = "";
		
		System.arraycopy(buffer, i, strBuffer, 0, Tag.CLIENT_TYPE_SIZE);
		i = i + Tag.CLIENT_TYPE_SIZE;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		clientType = temp;
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, Tag.CONTROL_SIGNAL_SIZE);
		i = i + Tag.CONTROL_SIGNAL_SIZE;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		control = temp;
		Arrays.fill(strBuffer, (byte)0);
		
		
		System.arraycopy(buffer, i, strBuffer, 0, 4);
		i = i + 4;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		yy = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, 2);
		i = i + 2;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		mm = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, 2);
		i = i + 2;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		dd = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, 2);
		i = i + 2;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		hour = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, 2);
		i = i + 2;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		minute = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, 2);
		i = i + 2;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		second = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
				
		System.arraycopy(buffer, i, strBuffer, 0, Tag.SENSORHUB_NAME_SIZE);
		temp = "";
		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		SensorHubName = temp;
		i = i + Tag.SENSORHUB_NAME_SIZE;
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, Tag.SENSOR_ID_SIZE);
		temp = "";

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		SensorID = temp;
		i = i + Tag.SENSOR_ID_SIZE;
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, Tag.SENSOR_TYPE_SIZE);
		temp = "";

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}
		SensorType = temp;
		i = i + Tag.SENSOR_TYPE_SIZE;
		Arrays.fill(strBuffer, (byte)0);
		
		System.arraycopy(buffer, i, strBuffer, 0, 5);
		i = i + 5;

		try
		{
			temp = new String(strBuffer, "US-ASCII");
		}
		catch(UnsupportedEncodingException ex)
		{
			System.out.println("Unhandled encoding.\n");
		}

		value = Integer.parseInt(temp.trim());
		Arrays.fill(strBuffer, (byte)0);
	}
	
	@Override
	public String toString()
	{
		return String.format("%s%d/%d/%d %d:%d:%d\n%s%s%s%s%s%s%s%s%s%s%s%s%d%s",
				"Time:\n\t", yy, mm, dd, hour, minute, second,
				"Client type:\n\t", clientType, "\nControl:\n\t", control,
				"\nSensorInfo:\n\t", "SensorHubName:", SensorHubName, "\n\tSensorID: ", SensorID, "\n\tSensorType: ", SensorType,
				"\n\tSensor Value: ", value, "\n");
	}
}
