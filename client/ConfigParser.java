import java.io.*;

public class ConfigParser{

    private String controllerAddress;
    private int controllerPort;
    private int displayTimeoutValue;
    private String ressources;

    public ConfigParser(String fileName){

	//lecture du fichier texte	
	try{
	    InputStream ips=new FileInputStream(fileName); 
	    InputStreamReader ipsr=new InputStreamReader(ips);
	    BufferedReader br=new BufferedReader(ipsr);
	    String line;
	    while ((line=br.readLine())!=null){
		if(line.charAt(0) != '#' && line != "\n"){
		    String[] words = line.split(" ");
		    switch (words[0]){
		    case "controller-address":
			controllerAddress = words[2];
			break;
		    case "controller-port":
			controllerPort = Integer.parseInt(words[2]);
			break;
		    case "display-timeout-value":
			displayTimeoutValue = Integer.parseInt(words[2]);
			break;
		    case "resources":
			ressources = words[2];
			break;
		    case "\n":
			break;
		    default:
			System.out.println("Error in "+ fileName);
			break;
		    }
		}
	    }
	    br.close();
	}		
	
	catch (Exception e){
	    System.out.println(e.toString());
	}
    }

    public String getControllerAddress(){
	return controllerAddress;
    }

    public int getControllerPort(){
	return controllerPort;
    }

    public int getDisplayTimeoutValue(){
	return displayTimeoutValue;
    }

    public String getRessources(){
	return ressources;
    }	
}
