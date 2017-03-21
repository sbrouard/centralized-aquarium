import java.net.*;
import java.io.*;

public class AccepterConnexion implements Runnable {

    private Socket client = null;
    private ServerSocket socketServeur = null;
    private PrintWriter out = null;
    private BufferedReader in = null;
    private Thread t1 = null;
    
    public AccepterConnexion(ServerSocket s){
	socketServeur = s;
    }

    public void run(){
	try{
	    while(true){
		client = socketServeur.accept();
		System.out.println("Un client souhaite se connecter");

		t1 = new Thread(new ChatClientServeur(client, "blabal"));
		t1.start();
	    }
	    
	} catch(IOException e){
	    e.printStackTrace();
	} 
    }
    
}
