var ambrosys_libs = {};

ambrosys_libs.init = function() 
{ 

};

ambrosys_libs.init_window = function(windowid) 
{ 
    self.postMessage('init_window('+windowid+');');
};

ambrosys_libs.switch_draw_buffer =  function(windowid) 
{ 
    self.postMessage('switch_draw_buffer(' + windowid + ');');
};

ambrosys_libs.draw_point =  function(windowid, x, y, color_r, color_g, color_b) 
{
    self.postMessage('draw_point('+windowid+ ', ' + x + ', ' + y + ', ' + color_r +', ' + color_g +', ' + color_b +');');
};


ambrosys_libs.get_cycel_waittime = function() 
{ 
    self.postMessage('worker.postMessage(\'\'+get_cycel_waittime());');
    return 100;
};


mergeInto(LibraryManager.library, ambrosys_libs);


