import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Emission implements Runnable {

    private Socket socket;
    
    public Reception(Socket s){
	socket = s;
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
		while( ( nextOpenBracket = line.indexOf('[',nextOpenBraket + 1) ) != -1){
		    nextCloseBracket = line.indexOf(']',nextCloseBracket +1);
		    String fish = line.subtring(nextOpenBracket + 1,nextCloseBracket);
		    // fish is of the form: "GolgFish at 92x40,10x4,5"
		    String[] fishInfos = fish.split(" ");
		    if (fishInfos.length != 3)
			return false;
		    else{
			String fishName = fishInfos[0]; //"GoldFish"
			String[] fishValues = fishInfos[2].split(","); //"92x40";"10x4";"5"
			if(fishValues.length != 3)
			    return false;
			else{
			    String[] fishNewCoords = fishValues[0].split("x");// "92";"40"
			    String[] fishSize = fishValues[1].split("x"); //"10";"4"
			    String fishTime = fishValues[2]; // "5"
			    String fishNewPosX = fishNewCoords[0]; // "92"
			    String fishNewPosY = fishNewCoords[1]; // "40"
			    String fishWidth = fishSize[0]; // "10"
			    String fishHeight = fishSize[1]; // "4"

			    updateFish(fishName,fishNewPosX,fishNewPosY,fishWidth,fishHeight); // TODO
			}
		    }
		}
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

}
