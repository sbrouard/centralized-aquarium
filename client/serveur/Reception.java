import java.net.*;
import java.io.*;

public class Reception implements Runnable {

    private BufferedReader in;
    private PrintWriter out;
    private String login, msg = null;
    
    public Reception(BufferedReader in, PrintWriter out, String login){
	this.in = in;
	this.out = out;
	this.login = login;
    }

    public void run(){
	try{
	    while(true){
		msg = in.readLine();
		System.out.println(login + "a envoye : " + msg);
		out.println(login + "a envoye : " + msg);
	    }
	} catch(IOException e){
	    e.printStackTrace();
	}
    }
}
