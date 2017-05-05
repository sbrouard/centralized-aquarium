import java.net.*;
import java.io.*;
import java.lang.*;

public class Client {

    public static void main(String args[]){
	
	Socket socket = null;
	Thread t = null;
	ConfigParser cfg = new ConfigParser("affichage.cfg");
	String address = cfg.getControllerAddress();
	int port = cfg.getControllerPort();
	
	try {
	    
	    socket = new Socket(address, port);
	    	    
	    t = new Thread(new Emission(socket, cfg.getDisplayTimeoutValue()));
	    t.start();
	    
	} catch(IOException e){
	    System.err.println("Aucun serveur a l'ecoute du port " + port + " à l'adresse " + address + ".");
	    System.err.println("Veuillez vérifier les données du fichier affichage.cfg, ou bien démarrer un serveur à cette addresse.");
	    System.err.println("Arrêt du programme.");
	    System.exit(1);
	}

	return;
    }

}
