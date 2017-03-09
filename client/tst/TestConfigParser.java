public class TestConfigParser {
    
    public void testParser(){
	ConfigParser cfgp = new ConfigParser("affichage.cfg");
	//assert("127.0.0.1".equals(cfgp.getControllerAddress()));
	//assert(cfgp.getControllerPort() == 12345);
	//assert(cfgp.getDisplayTimeoutValue() == 30);
	//	assert(cfgp.getRessources().equals("./fishes"));
	
	System.out.print(".");
    }

    public static void main(String args[]){
	
	boolean estMisAssertion = false;
	assert estMisAssertion = true;

	if (!estMisAssertion){
		System.out.println("Execution impossible sans l'option -ea");
		return;
	}

	TestConfigParser test = new TestConfigParser();
	test.testParser();
	System.out.println(" OK");
    }

}
