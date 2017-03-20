import java.net.*;
import java.io.*;

public class Client {

    public static void main(String args[]){
	
	Socket socket = null;
	Thread t = null;
	
	try {
	    ConfigParser cfg = new ConfigParser("affichage.cfg");
	    String address = cfg.getControllerAddress();
	    int port = cfg.getControllerPort();
	    socket = new Socket(address, port);
	    
	    t = new Thread(new Connexion(socket));
	    t.start();
	    
	} catch(UnknownHostException e){
	    System.err.println("Impossible de se connecter a l'adresse " + socket.getLocalAddress());
	} catch(IOException e){
	    System.err.println("Aucun serveur a l'ecoute du port " + socket.getLocalPort());
	} 
	
    }

}
