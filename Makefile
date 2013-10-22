INCLUDES = -I$(ODEINT_ROOT) -I$(BOOST_ROOT)
OPTIONS = -O2 -std=c++11 -Wno-warn-absolute-paths

molecular_dynamics.js: molecular_dynamics_js_to_c.js molecular_dynamics.cpp
	em++ molecular_dynamics.cpp $(INCLUDES) $(OPTIONS) --js-library molecular_dynamics_js_to_c.js -o molecular_dynamics.js

tar:
	cd .. && tar cjf molecular_dynamics_js.tar.bz2 molecular_dynamics/molecular_dynamics.cpp molecular_dynamics/molecular_dynamics.html molecular_dynamics/molecular_dynamics.js molecular_dynamics/molecular_dynamics.js.map molecular_dynamics/molecular_dynamics_js_to_c.js
