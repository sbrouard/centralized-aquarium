import java.net.*;
import java.io.*;

public class Serveur {

    public static ServerSocket serveurSocket = null;
    public static Thread t;
    
    public static void main(String args[]) {

       	try {
	    serveurSocket = new ServerSocket(8080);

	    t = new Thread(new AccepterConnexion(serveurSocket));
	    t.start();
	    System.out.println("Le serveur est lance et ecoute le port 8080...");
	    
	} catch(IOException e){
	    System.err.println("Le port n° " + serveurSocket.getLocalPort() + " est deja utilise");
	}
	
    }
    
}
