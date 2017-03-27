import java.net.*;
import java.io.*;
import java.util.Scanner;
import java.util.ArrayList;

public class Reception implements Runnable {

    private Socket socket;
    private Aquarium aquarium;
    
    public Reception(Socket s){
	socket = s;
	aquarium = new Aquarium();
	Thread t4 = new Thread(new Display(aquarium, 50));
	t4.start();
    }

    private boolean parser(String line){
	if(line.length() == 0)
	    return false;
	else{
	    String[] words = line.split(" ");
	    switch (words[0]){
		//OK
	    case "OK":
		return true;
		
		//NOK
	    case "NOK":
		return true;

		// getFishes -> list [...] ... [...]
	    case "list":
		//int indexOf(String  str, int fromIndex)
		//String substring(int d, int f)
		int nextOpenBracket = -1;
		int nextCloseBracket = -1;
		ArrayList<Fish> fishes = new ArrayList<Fish>();
		while( ( nextOpenBracket = line.indexOf('[',nextOpenBracket + 1) ) != -1){
		    nextCloseBracket = line.indexOf(']',nextCloseBracket +1);
		    String fish = line.substring(nextOpenBracket + 1,nextCloseBracket);
		    // fish is of the form: "GolgFish at 92x40,10x4,5"
		    String[] fishInfos = fish.split(" ");
		    if (fishInfos.length != 3)
			return false;
		    else{
			String fishName = fishInfos[0]; //"GoldFish"
			String fishType = (fishName.split("_"))[0];
			String[] fishValues = fishInfos[2].split(","); //"92x40";"10x4";"5"
			if(fishValues.length != 3)
			    return false;
			else{
			    String[] fishNewCoords = fishValues[0].split("x");// "92";"40"
			    String[] fishSize = fishValues[1].split("x"); //"10";"4"
			    String fishTime = fishValues[2]; // "5"
			    int fishNewPosX = Integer.parseInt(fishNewCoords[0]); // "92"
			    int fishNewPosY = Integer.parseInt(fishNewCoords[1]); // "40"
			    int fishWidth = Integer.parseInt(fishSize[0]); // "10"
			    int fishHeight = Integer.parseInt(fishSize[1]); // "4"
			    int time = Integer.parseInt(fishTime);
			    Fish newFish = new Fish(fishName, fishType, fishNewPosX, fishNewPosY, fishWidth, fishHeight, time);
			    fishes.add(newFish);
			    System.out.println("Poissonn ajoute : " + fishes.toString());
			}
		    }
		}
	        aquarium.setFishes(fishes);
		return true;

		// pong
	    case "pong":
		return true;
		//else: error
	    default:
		return false;

	    }
	}
    }

    public void run(){
       	try{

	    while(true){
		BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		String msg = in.readLine();
		System.out.println(msg);
		parser(msg);
	    }

	} catch(IOException e){
	    e.printStackTrace();
	}
    }

}

