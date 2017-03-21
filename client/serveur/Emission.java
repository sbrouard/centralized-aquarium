import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Emission implements Runnable {

    private PrintWriter out;
    private Scanner sc;
    private String msg = null;
    
    public Emission(PrintWriter out){
	this.out = out;
    }

    public void run(){
	sc = new Scanner(System.in);

	while(true){
	    System.out.println("Entrez votre message :");
	    msg = sc.nextLine();
	    out.println(msg);
	    out.flush();
	}
    }
}
