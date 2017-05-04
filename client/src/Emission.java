import java.net.*;
import java.io.*;
import java.util.Scanner;
import java.util.concurrent.*;

public class Emission implements Runnable {

    private Socket socket;
    private int timePing;
    
    public Emission(Socket s, int timePing){
	socket = s;
	this.timePing = timePing;
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

    private boolean correctSyntax(String cmd){
	String[] words = cmd.split(" ");
	if(words.length == 0)
	    return false;
	else if (!words[0].equals("hello"))
	    return false;
	else if ( words.length == 1)
	    return true;
	else if (words.length != 4)
	    return false;
	else if (words[1] == "in" && words[2] == "as")
	    return true;
	else 
	    return false;
    }
    
    public void run(){

	try{
	
	    final PrintWriter out = new PrintWriter(socket.getOutputStream());
	    final BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
	    
	    boolean connected = false;
	    final Scanner sc = new Scanner(System.in);
	    String cmd = sc.nextLine();

	    /*
	     * TRYING TO CONNECT TO SERVER
	     */
	    while(!connected ){
		while(!correctSyntax(cmd)){
		    System.err.println("First use 'hello [in as ID]'");
		    cmd = sc.nextLine();
		}
		    
		out.println(cmd);
		out.flush();

		String rep = in.readLine();
		System.out.println(rep);
		String[] words_rep = rep.split(" ");
		if(words_rep[0].equals("greeting")){
		    connected = true;
		    System.out.println("Vous êtes connecté");
		}
		else
		    System.err.println("Server did not accept the connection");
		
	    }

	    /*
	     * TO RECEIVE MESSAGES FROM SERVER
	     */
	    Reception r = new Reception(socket);
	    Thread t2 = new Thread(r);
	    t2.start();
	
	    /*
	     * TO PING EVERY X SECONDS
	     */	    
	    final ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();
	    executorService.scheduleAtFixedRate(new Runnable() {
		    @Override
		    public void run() {
			out.println("ping");
			out.flush();
		    }
		}, 0, timePing, TimeUnit.SECONDS);

	    /*
	     * TO SEND MESSAGES TO SERVER
	     */	
	    while(true){
		System.out.print("> ");
		cmd = sc.nextLine();
		out.println(cmd);
		out.flush();
	    }

	} catch (IOException e){
	    System.err.println("le serveur ne répond pas. Arrêt du programme.");
	    System.exit(1);
	}

	return;
    }
}
