import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Emission implements Runnable {

    private Socket socket;
    
    public Emission(Socket s){
	socket = s;
    }

    /*
    private boolean correctSyntax(String cmd){
	String[] words = cmd.split(" ");
	if(words.length() == 0 || words.length > 2)
	    return false;
	else if (words.length == 1){
	    if(words[0].equals("status"))
		return true;
	    else if(words[0].equals("getFishes"))
		return true;
	    else 
		return false;

	}
	else if (words.length == 2){
	    if(words[0].equals("getFishes") && words[1].equals("Continuously"))
		return true;
	    else if(words[0].equals("log") && words[1].equals("out"))
		return true;
	    else 
		return false;
	}
	return true;
	// pas fini d'implementer car c'est au serveur de le faire, bon chance !

    }
    */

    public void run(){
	
	Scanner sc = new Scanner(System.in);
	String cmd;
	PrintWriter out = new PrintWriter(socket.getOutputStream());

	while(true){
	    cmd = sc.nextLine();
	    out.println(cmd);
	}
    }
}
