camera_load_config("redcup");
        camera_open();
        camera_update();
        if (get_object_count() < 1){
            return 1;
        }
        int x = get_object_center(0,0);
        if (x >= 400){
            return 3;
        }else if ( x <= 200){
            return 2;
        }else{
            return 1;
        }
        
