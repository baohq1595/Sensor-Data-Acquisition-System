import java.awt.Color;
import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.SpringLayout;
import javax.swing.table.DefaultTableModel;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextField;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.SwingConstants;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.awt.event.ActionEvent;
import javax.swing.JComboBox;

public class MainWindows {

	private JFrame frame;
	private JScrollPane scrollPaneTable;
	private JTable table;
	private JLabel lblIp;
	private JTextField textFieldIPAddr;
	private JLabel lblPort;
	private JTextField textFieldPort;
	private JButton btnConnect;
	private JButton btnUpdate;
	private JLabel lblConnectStatus;
	private JLabel lblUpdateStatus;
	private JComboBox<String> comboBox;
	private JButton btnNewButton;
	
	private static ClientSide client;
	private static ArrayList<Package> packageList;
	private static ExecutorService thread;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainWindows window = new MainWindows();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public MainWindows() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		
		thread = Executors.newCachedThreadPool();
		client = new ClientSide();
		
		frame = new JFrame();
		frame.setBackground(Color.WHITE);
		frame.setBounds(100, 100, 751, 438);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		SpringLayout springLayout = new SpringLayout();
		frame.getContentPane().setLayout(springLayout);
		frame.setTitle("Control");
		
		scrollPaneTable = new JScrollPane();
		springLayout.putConstraint(SpringLayout.NORTH, scrollPaneTable, 102, SpringLayout.NORTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.WEST, scrollPaneTable, 23, SpringLayout.WEST, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.SOUTH, scrollPaneTable, -10, SpringLayout.SOUTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.EAST, scrollPaneTable, -10, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(scrollPaneTable);
		
		table = new JTable();
		scrollPaneTable.setViewportView(table);
		springLayout.putConstraint(SpringLayout.SOUTH, table, -10, SpringLayout.SOUTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.EAST, table, 486, SpringLayout.WEST, frame.getContentPane());
		
		lblIp = new JLabel("IP");
		springLayout.putConstraint(SpringLayout.NORTH, lblIp, 6, SpringLayout.NORTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.WEST, lblIp, 31, SpringLayout.WEST, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.SOUTH, lblIp, -66, SpringLayout.NORTH, scrollPaneTable);
		springLayout.putConstraint(SpringLayout.EAST, lblIp, -690, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(lblIp);
		
		textFieldIPAddr = new JTextField();
		springLayout.putConstraint(SpringLayout.NORTH, textFieldIPAddr, 0, SpringLayout.NORTH, lblIp);
		springLayout.putConstraint(SpringLayout.WEST, textFieldIPAddr, 18, SpringLayout.EAST, lblIp);
		springLayout.putConstraint(SpringLayout.SOUTH, textFieldIPAddr, -66, SpringLayout.NORTH, scrollPaneTable);
		frame.getContentPane().add(textFieldIPAddr);
		textFieldIPAddr.setColumns(10);
		
		lblPort = new JLabel("Port");
		springLayout.putConstraint(SpringLayout.NORTH, lblPort, 6, SpringLayout.NORTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.SOUTH, lblPort, -66, SpringLayout.NORTH, scrollPaneTable);
		springLayout.putConstraint(SpringLayout.EAST, textFieldIPAddr, -26, SpringLayout.WEST, lblPort);
		springLayout.putConstraint(SpringLayout.EAST, lblPort, 273, SpringLayout.WEST, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.WEST, lblPort, 242, SpringLayout.WEST, frame.getContentPane());
		frame.getContentPane().add(lblPort);
		
		textFieldPort = new JTextField();
		springLayout.putConstraint(SpringLayout.NORTH, textFieldPort, 6, SpringLayout.NORTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.WEST, textFieldPort, 28, SpringLayout.EAST, lblPort);
		springLayout.putConstraint(SpringLayout.SOUTH, textFieldPort, -66, SpringLayout.NORTH, scrollPaneTable);
		springLayout.putConstraint(SpringLayout.EAST, textFieldPort, -301, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(textFieldPort);
		textFieldPort.setColumns(10);
		
		btnConnect = new JButton("Connect");
		btnConnect.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String ip = textFieldIPAddr.getText();
				String port = textFieldPort.getText();
				
				if (ip.equals("") || port.equals(""))
				{
					JOptionPane.showMessageDialog(frame, "Input IP address and port number.");
				}
				else
				{
					ClientSide client = new ClientSide(ip, Integer.parseInt(port));
					client.getConnection();
					if (client.getConnectionStatus() == true)
					{
						lblConnectStatus.setText("Connected.");
					}
					else
					{
						lblConnectStatus.setText("Error.");
					}
				}
			}
		});
		springLayout.putConstraint(SpringLayout.NORTH, btnConnect, 6, SpringLayout.SOUTH, textFieldIPAddr);
		springLayout.putConstraint(SpringLayout.WEST, btnConnect, 63, SpringLayout.WEST, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.SOUTH, btnConnect, 36, SpringLayout.SOUTH, textFieldIPAddr);
		springLayout.putConstraint(SpringLayout.EAST, btnConnect, -42, SpringLayout.EAST, textFieldIPAddr);
		frame.getContentPane().add(btnConnect);
		
		btnUpdate = new JButton("Update");
		btnUpdate.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String connectionStatus = lblConnectStatus.getText();
				if (connectionStatus.equals("Connected."))
				{
					lblUpdateStatus.setText("Updating...");
					
					//Wait for data update
					client.run();
					
					//Update data
					packageList = client.getPackageList();
					DefaultTableModel model = (DefaultTableModel)table.getModel();
					for (int i = 0; i < packageList.size(); i++)
					{
						model.addRow(new Object[]{String.valueOf(i), packageList.get(i).getTime(), packageList.get(i).getSensorHubName(), 
								packageList.get(i).getSensorID(), packageList.get(i).getSensorType(), packageList.get(i).getValue()});
					}
					if (client.getUpdateStatus() == true)
					{
						lblUpdateStatus.setText("Updated!");
					}
						
				}
				else
				{
					JOptionPane.showMessageDialog(frame, "Connection error.");
					lblUpdateStatus.setText("Error.");
				}
			}
		});
		springLayout.putConstraint(SpringLayout.NORTH, btnUpdate, 6, SpringLayout.SOUTH, textFieldPort);
		springLayout.putConstraint(SpringLayout.SOUTH, btnUpdate, -30, SpringLayout.NORTH, scrollPaneTable);
		springLayout.putConstraint(SpringLayout.EAST, btnUpdate, -341, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(btnUpdate);
		
		lblConnectStatus = new JLabel("");
		lblConnectStatus.setHorizontalAlignment(SwingConstants.CENTER);
		springLayout.putConstraint(SpringLayout.EAST, lblConnectStatus, -440, SpringLayout.EAST, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.WEST, btnUpdate, 6, SpringLayout.EAST, lblConnectStatus);
		springLayout.putConstraint(SpringLayout.SOUTH, lblConnectStatus, -30, SpringLayout.NORTH, scrollPaneTable);
		springLayout.putConstraint(SpringLayout.NORTH, lblConnectStatus, 6, SpringLayout.SOUTH, lblPort);
		springLayout.putConstraint(SpringLayout.WEST, lblConnectStatus, 6, SpringLayout.EAST, btnConnect);
		frame.getContentPane().add(lblConnectStatus);
		
		lblUpdateStatus = new JLabel("");
		lblUpdateStatus.setHorizontalAlignment(SwingConstants.CENTER);
		springLayout.putConstraint(SpringLayout.WEST, lblUpdateStatus, 6, SpringLayout.EAST, btnUpdate);
		springLayout.putConstraint(SpringLayout.NORTH, lblUpdateStatus, 42, SpringLayout.NORTH, frame.getContentPane());
		springLayout.putConstraint(SpringLayout.SOUTH, lblUpdateStatus, 0, SpringLayout.SOUTH, btnConnect);
		springLayout.putConstraint(SpringLayout.EAST, lblUpdateStatus, -195, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(lblUpdateStatus);
		
		table.setModel(new DefaultTableModel(
			new Object[][] {	
			},
				new String[] {
					"No.", "Time", "SensorHub", "SensorID", "SensorType", "Value"
				}
			)
		);
		table.getColumnModel().getColumn(0).setPreferredWidth(15);
		//ComboBox and Button Search
		comboBox = new JComboBox<String>();
		springLayout.putConstraint(SpringLayout.NORTH, comboBox, 0, SpringLayout.NORTH, lblIp);
		springLayout.putConstraint(SpringLayout.WEST, comboBox, 50, SpringLayout.EAST, textFieldPort);
		springLayout.putConstraint(SpringLayout.SOUTH, comboBox, 0, SpringLayout.SOUTH, lblIp);
		springLayout.putConstraint(SpringLayout.EAST, comboBox, -93, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(comboBox);
		
		btnNewButton = new JButton("Search");
		springLayout.putConstraint(SpringLayout.WEST, btnNewButton, 6, SpringLayout.EAST, comboBox);
		springLayout.putConstraint(SpringLayout.NORTH, btnNewButton, 0, SpringLayout.NORTH, lblIp);
		springLayout.putConstraint(SpringLayout.SOUTH, btnNewButton, 0, SpringLayout.SOUTH, lblIp);
		springLayout.putConstraint(SpringLayout.EAST, btnNewButton, -10, SpringLayout.EAST, frame.getContentPane());
		frame.getContentPane().add(btnNewButton);
		
		DefaultComboBoxModel<String> modelCombo=new DefaultComboBoxModel<String>();	
		modelCombo.addElement("HUB01");
		modelCombo.addElement("LIGHT");
		modelCombo.addElement("TEMPA");
		modelCombo.addElement("HUMID");
		modelCombo.addElement("T001");
		modelCombo.addElement("L001");
		modelCombo.addElement("H001");
		modelCombo.addElement("11/12/2016");
		comboBox.setModel(modelCombo);
		
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				DefaultTableModel model=(DefaultTableModel)table.getModel();	
				while(model.getRowCount()!=0)
			       {
					model.removeRow(0);
			       }
					 ArrayList<Package> packageList_temp = packageList;
					for (int i = 0; i < packageList_temp.size(); i++)
					{
						if(comboBox.getSelectedItem().toString()== packageList_temp.get(i).getSensorHubName()){
						model.addRow(new Object[]{model.getRowCount()+1, packageList_temp.get(i).getTime(), packageList_temp.get(i).getSensorHubName(), 
								packageList_temp.get(i).getSensorID(), packageList_temp.get(i).getSensorType(), packageList_temp.get(i).getValue()});
						}
						else if(comboBox.getSelectedItem().toString()== packageList_temp.get(i).getSensorID()){
							model.addRow(new Object[]{model.getRowCount()+1, packageList_temp.get(i).getTime(), packageList_temp.get(i).getSensorHubName(), 
									packageList_temp.get(i).getSensorID(), packageList_temp.get(i).getSensorType(), packageList_temp.get(i).getValue()});
							}
						else if(comboBox.getSelectedItem().toString()== packageList_temp.get(i).getSensorType()){
							model.addRow(new Object[]{model.getRowCount()+1, packageList_temp.get(i).getTime(), packageList_temp.get(i).getSensorHubName(), 
									packageList_temp.get(i).getSensorID(), packageList_temp.get(i).getSensorType(), packageList_temp.get(i).getValue()});
							}
						else if(comboBox.getSelectedItem().toString()== packageList_temp.get(i).getTime()){
							model.addRow(new Object[]{model.getRowCount()+1, packageList_temp.get(i).getTime(), packageList_temp.get(i).getSensorHubName(), 
									packageList_temp.get(i).getSensorID(), packageList_temp.get(i).getSensorType(), packageList_temp.get(i).getValue()});
							}
					}
			}
		});

		
	}
}
