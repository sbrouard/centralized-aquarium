import java.net.*;
import java.io.*;

public class ChatClientServeur implements Runnable{
    private BufferedReader in = null;
    private PrintWriter out = null;
    private Thread t3 = null, t4 = null;
    private Socket client = null;
    private String login = null;
    
    public ChatClientServeur(Socket s, String login){
	client = s;
	this.login = login;
    }

    public void run(){
	try{
	    in = new BufferedReader(new InputStreamReader(client.getInputStream()));
	    out = new PrintWriter(client.getOutputStream());

	    t3 = new Thread(new Reception(in, out, login));
	    t3.start();

	    t4 = new Thread(new Emission(out));
	    t4.start();
	    
	} catch(IOException e){
	    System.err.println(login + " s'est deconnecte");
	}
    }
}
