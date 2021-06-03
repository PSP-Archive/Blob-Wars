class Keyboard {

	public:
		
			int left, right, down;
			int fire, jump, jetpack;
			int pause, map;
	
		Keyboard();
		void setDefaultKeys();
		static char *Keyboard::translateKey(int value);

};
