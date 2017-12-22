package aphereon.tut.pingpong;

import aphereon.base.system.AOSActivity;

public class PingPongMainActivity extends AOSActivity {

	static 
	{
		// load library
		System.loadLibrary("PingPong");
	}
	
	@Override
	public String getNativeActivityClass() {
		// TODO Auto-generated method stub
		return "TutorialHomeActivity";
	}

    
    
}
