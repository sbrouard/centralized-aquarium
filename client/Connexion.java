import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Connexion implements Runnable {

    private Socket socket;
    
    public Connexion(Socket s){
	socket = s;
    }

    private boolean correctSyntax(String cmd){
	String[] words = cmd.split(" ");
	if(words.length() == 0)
	    return false;
	else if (!words[0].equals("hello"))
	    return false;
	else if ( words.length() == 1)
	    return true;
	else if (words.length() != 4)
	    return false;
	else if (words[1] == "in" && words[2] == "as")
	    return true;
	else 
	    return false;
    }

    public void run(){
	try {
	    boolean connected = false;
	    Scanner sc = new Scanner(System.in);
	    String cmd = sc.nextLine();
	    while(!connected ){
		
		while(!correctSyntax(cmd)){
		    System.err.println("First use 'hello [in as ID]'");
		    cmd = sc.nextLine();
		}

		PrintWriter out = new PrintWriter(socket.getOutputStream());
		BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

		out.printl(cmd);
	    
		String rep = in.readLine();
		String[] words_rep = rep.split(" ");
		if(words_rep[0].equals("greeting"))
		    connected = true;
		else
		    System.err.println("Server did not accept the connection");
	    
	    }
	    
	    Thread t2 = new Thread(new Emission(socket));
	    Thread t3 = new Thread(new Reception(socket));
	    t2.start();
	    t3.start();
	    
	} catch(IOException e){
	    System.err.println("le serveur ne repond pas");
	}
    }    
}