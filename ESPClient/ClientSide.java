
import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.TimeUnit;

import javax.swing.JTable;
import javax.swing.SwingWorker;
import javax.swing.table.DefaultTableModel;

public class ClientSide extends Thread{
	private static String host;
	private static int port;
	private static Socket socket;
	private static DataInputStream inStream;
	private static DataOutputStream outStream;
	private static String state;
	private static boolean isUpdateCmd;
	private static boolean isUpdated;
	private static boolean isConnected;
	
	private static ArrayList<Package> packList;
	
	public ClientSide()
	{
		this.isConnected = false;
		this.isUpdateCmd = false;
		this.isUpdated = false;
		this.state = Tag.INIT;
	}
	
	public ClientSide(String host, int port)
	{
		this.host = host;
		this.port = port;
		this.isConnected = false;
		this.isUpdateCmd = false;
		this.isUpdated = false;
		this.state = Tag.INIT;
		packList = new ArrayList<Package>(100);
	}
	
	public void setIPAddr(String ipAddr)
	{
		this.host = ipAddr;
	}
	
	public void serPort(int port)
	{
		this.port = port;
	}
	
	public boolean getConnectionStatus()
	{
		return this.isConnected;
	}
	
	public void setUpdateCmd(boolean value)
	{
		this.isUpdateCmd = true;
	}
	
	public boolean getUpdateStatus()
	{
		return this.isUpdated;
	}
	
	public ArrayList<Package> getPackageList()
	{
		return packList;
	}
	
	public void getConnection()
	{
		try
		{
			socket = new Socket(host, port);
			
			inStream = new DataInputStream(socket.getInputStream());
			outStream = new DataOutputStream(socket.getOutputStream());
			outStream.flush();
			isConnected = true;
		}
		catch (IOException ex)
		{
			System.out.println("Error creating socket.\n");
			isConnected = false;
		}
	}
	
	public void run()
	{
		//getConnection();
		int byteRead = 0;
		byte [] recvBuffer = new byte [Tag.PACKAGE_MAX_SIZE];
		byte [] sendBuffer = new byte [Tag.PACKAGE_MAX_SIZE];
		isUpdated = false;
		//BufferedReader stdIn = new BufferedReader(new 
		//		InputStreamReader(System.in));
		
		try
		{
			/*
			 * Protocol for update data packages
			 * Send UPDATE_DATA requirement
			 * Wait for response from server - READY_TO_SEND
			 * Send GET_DATA signal to server (1)
			 * Wait for incoming data package (2)
			 * Loop (1) and (2) until receive END_OF_FILE signal
			 * Wait for user signal to update data or until timeout  
			 */
			
			String mssg1, mssg2;
			byte []tmp1 = new byte[Tag.CLIENT_TYPE_SIZE];
			byte []tmp2 = new byte[Tag.CONTROL_SIGNAL_SIZE];
			while(true)
			{
				if (isUpdated == true)
					break;
				Arrays.fill(sendBuffer, (byte)0);
				Arrays.fill(recvBuffer, (byte)0);
				Arrays.fill(tmp1, (byte)0);
				Arrays.fill(tmp2, (byte)0);
				switch(state)
				{
					case Tag.INIT:
						//Send UPDATE_DATA to server
						isUpdated = false;
						createPackageForCmd(Tag.PC_CLIENT, Tag.UPDATE_DATA, sendBuffer);
						try
						{
							TimeUnit.MILLISECONDS.sleep(250);
						}
						catch (InterruptedException ex)
						{
							
						}
						outStream.write(sendBuffer);
						outStream.flush();
						
						//Wait for server to send READY signal to go to next state
						byteRead = inStream.read(recvBuffer, 0, Tag.PACKAGE_MAX_SIZE);
						while (byteRead < Tag.PACKAGE_MAX_SIZE)
						{
							byteRead = inStream.read(recvBuffer, byteRead, Tag.PACKAGE_MAX_SIZE);
						}
						byteRead = 0;
						System.arraycopy(recvBuffer, 0, tmp1, 0, Tag.CLIENT_TYPE_SIZE);
						System.arraycopy(recvBuffer, Tag.CLIENT_TYPE_SIZE, tmp2, 0, Tag.CONTROL_SIGNAL_SIZE);
						mssg1 = new String(tmp1, "US-ASCII");
						mssg2 = new String(tmp2, "US-ASCII");
						
						System.out.println("Response from server:" + mssg1 + "----" + mssg2);
						
						if (mssg1.equals(Tag.SERVER) && mssg2.equals(Tag.READY))
						{
							//Send get data signal to server
							Arrays.fill(sendBuffer, (byte)0);
							createPackageForCmd(Tag.PC_CLIENT, Tag.GET_DATA, sendBuffer);
							try
							{
								TimeUnit.MILLISECONDS.sleep(250);
							}
							catch (InterruptedException ex)
							{
								
							}
							outStream.write(sendBuffer);
							outStream.flush();
							
							state = Tag.IN_PROGRESS;
						}
						else 
							state = Tag.INIT;
						break;
					case Tag.IN_PROGRESS:
						//Receive package from server
						byteRead = inStream.read(recvBuffer, 0, Tag.PACKAGE_MAX_SIZE);
						while (byteRead < Tag.PACKAGE_MAX_SIZE)
						{
							byteRead = inStream.read(recvBuffer, byteRead, Tag.PACKAGE_MAX_SIZE);
						}
						byteRead = 0;
						Arrays.fill(tmp1, (byte)0);
						Arrays.fill(tmp2, (byte)0);
						System.arraycopy(recvBuffer, 0, tmp1, 0, Tag.CLIENT_TYPE_SIZE);
						System.arraycopy(recvBuffer, Tag.CLIENT_TYPE_SIZE, tmp2, 0, Tag.CONTROL_SIGNAL_SIZE);
						mssg1 = new String(tmp1, "US-ASCII");
						mssg2 = new String(tmp2, "US-ASCII");
						if (mssg2.equals(Tag.SEND_DATA) && mssg1.equals(Tag.SERVER))
						{
							//Deserialize receive buffer to package
							Package tempPack = new Package();
							tempPack.Deserialize(recvBuffer);
							System.out.println("Packet received:\n" + tempPack);
							if (mssg1.equals(Tag.SERVER) && mssg2.equals(Tag.SEND_DATA))
							{
								//add received package to List
								packList.add(tempPack);
								
								//Send GETDATA requirement to server - get package OK
								createPackageForCmd(Tag.PC_CLIENT, Tag.GET_DATA, sendBuffer);
								try
								{
									TimeUnit.MILLISECONDS.sleep(250);
								}
								catch (InterruptedException ex)
								{
									
								}
								outStream.write(sendBuffer);
								outStream.flush();
								System.out.println("Sent to Server: --- " + new String(sendBuffer, "US-ASCII"));
							}
						}
						//Receive end-of-data signal
						else if (mssg2.equals(Tag.END_OF_DATA) && mssg1.equals(Tag.SERVER))
						{
							//Change state to waiting for user action
							state = Tag.WAITING;
							isUpdateCmd = false;
						}
						break;
					case Tag.WAITING:
						isUpdated = true;
						state = Tag.INIT;
						System.out.println("Packages updated.\n");
						break;
				}
			}
		}
		catch (IOException ex)
		{
			System.out.println("Error Sending/Receiving.\n");
			closeConnection();
			
		}
	}

	public void exceptionHandler()
	{
		//close all stream and socket
		closeConnection();

		//try to re-connect to Server
		getConnection();
		run();
	}
	
	public void closeConnection()
	{
		try
		{
			inStream.close();
			outStream.close();
			socket.close();
			
			System.out.println("Stream/socket closed.\n");
			
			System.exit(0);
		}
		catch (IOException ex)
		{
			System.out.println("Error closing stream/socket.\n");
		}
	}
	
	public void createPackageForCmd(String clientName, String control, byte[] buffer)
	{
		System.arraycopy(clientName.getBytes(), 0, buffer, 0, clientName.length());
		System.arraycopy(control.getBytes(), 0, buffer, Tag.CLIENT_TYPE_SIZE, control.length());
		Arrays.fill(buffer, Tag.CLIENT_TYPE_SIZE + Tag.CONTROL_SIGNAL_SIZE, Tag.PACKAGE_MAX_SIZE, (byte)0x23);
	}
}

