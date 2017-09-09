#include<switch.h>

/* GLOABLS */
static struct {
	char *odbc_dsn;
	char *dbname;
	char *redis_host;
	char *redis_port;
	char *redis_pswd;
	char *redis_timeout;
	switch_mutex_t *mutex; 
	switch_memory_pool_t *pool;
} globals;


/* Prototypes */
SWITCH_MODULE_SHUTDOWN_FUNCTION(test_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(test_runtime);
SWITCH_MODULE_LOAD_FUNCTION(test_load);


/* SWITCH_MODULE_DEFINITION(name, load, shutdown, runtime)
 * Defines a switch_loadable_module_function_table_t and a static const char[] modname
 */
SWITCH_MODULE_DEFINITION(mod_test, test_load, test_shutdown, NULL);


//load configurations from test1.cfg.xml
switch_status_t mod_test_config(){
	char *conf="test.cfg";
	switch_xml_t xml=NULL, cfg=NULL, settings,setting,db_settings,db_setting=NULL,redis_profiles,redis_profile=NULL,param;

	if (!(xml = switch_xml_open_cfg("test.conf", &cfg, NULL))) {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "open of %s failed\n", conf);
                goto err;
        }
	
	if( (settings = switch_xml_child(cfg,"settings")) != NULL){
		
		for(setting = switch_xml_child(settings,"setting");setting;setting = setting->next){
			char *name = (char *) switch_xml_attr_soft(setting, "name");
			
			if(!strncmp(name, "default",7)){
				switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"DEFAULT SETTING INFO\n");
			}	
		//load db_setting	
			if( (db_settings = switch_xml_child(setting,"db_setting")) != NULL){	
				switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"DB SETTING INFO\n");
				for(param = switch_xml_child(settings, "param"); param; param = param->next ){
					const char *name = switch_xml_attr(param, "name");
					const char *value = switch_xml_attr(param, "value");
				
					if((switch_strlen_zero(name) || switch_strlen_zero(value))){
						switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "BLANK PARAM VALUES\n");
						continue;
					}else{
			
						switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "%s \t %s\n",name,value);
						if(!strcasecmp(name,"odbc-dsn"))
						{
							 globals.odbc_dsn= strdup(value);
						}else if(!strcasecmp(name,"dbname")){
							globals.dbname = strdup(value);
						}
					}	

				}	
			}else{
				switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"ODBC setting is missing....\n");
				goto err;
			}
			
			
		//load redis_profile
			if( (redis_profiles = switch_xml_child(setting,"redis_profile")) != NULL ){
				switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"REDIS SETTING INFO\n");
				for(redis_profile = switch_xml_child(redis_profile,"param");param;param =param->next){
					 const char *name = switch_xml_attr(param, "name");
                                         const char *value = switch_xml_attr(param, "value");

                                        if((switch_strlen_zero(name) || switch_strlen_zero(value))){
                                                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "BLANK PARAM VALUES\n");
                                                goto err;
                                        }else{

                                                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "%s \t %s\n",name,value);
                                                if(!strcasecmp(name,"hostname"))
                                                {
                                                         globals.redis_host= strdup(value);
                                                }else if(!strcasecmp(name,"password")){
                                                                globals.redis_pswd = strdup(value);
                                                }else if(!strcasecmp(name,"port")){
                                                                globals.redis_port = strdup(value);
                                                }else if(!strcasecmp(name,"timeout_ms")){
                                                                globals.redis_timeout = strdup(value);
                                                }

                                        }


				}
			}else{
				switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Redis profile is missing....\n");
                                goto err;

			}
					
		}
		
	}


 return SWITCH_STATUS_SUCCESS;

 err:
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Configuration failed\n");
        return SWITCH_STATUS_GENERR;
}


SWITCH_MODULE_LOAD_FUNCTION(test_load)
{
	switch_status_t status;
//	switch_api_interface_t *api_interface;
//	switch_application_interface_t *app_interface;

 /* connect my internal structure to the blank pointer passed to me */
        *module_interface = switch_loadable_module_create_module_interface(pool, modname);

        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Welcome to Mod_test\n");

	/**
         * @Section initialize configuration structure.
         */

        memset(&globals, 0, sizeof(globals));
        globals.pool = pool;

        /**
         * @Section mutex initializing 
         */


	if((status = switch_mutex_init(&globals.mutex, SWITCH_MUTEX_NESTED, globals.pool))!= SWITCH_STATUS_SUCCESS){
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Do_config failed, %d\n",status);
	}else{
	
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "mutex::;, %d\n",status);
	}



      if( (status = mod_test_config(SWITCH_FALSE)) != SWITCH_STATUS_SUCCESS){
	 	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Do_config failed");
		return status;
	}else{       
		 switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Module configuration successfully loaded.\n");
	}

	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"#%s#%s#%s#%s#%s#%s\n",globals.odbc_dsn,globals.dbname,globals.redis_host,globals.redis_port,globals.redis_pswd,globals.redis_timeout);
        return SWITCH_STATUS_SUCCESS;
}


SWITCH_MODULE_SHUTDOWN_FUNCTION(test_shutdown)
{
        /* Cleanup dynamically allocated config settings */
  //      switch_xml_config_cleanup(instructions);
	 return SWITCH_STATUS_SUCCESS;
}

