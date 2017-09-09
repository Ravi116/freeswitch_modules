#include<switch.h>

/* GLOABLS */
static struct {
	char *odbc_dsn;
	char *dbname;
	int looping;
	switch_mutex_t *mutex; 
	switch_memory_pool_t *pool;

} globals;


/*     Function Declaration     */
static switch_cache_db_handle_t *switch_get_db_handler(void);
//static switch_bool_t switch_execute_sql_callback(switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback, void *pdata);
static char *switch_execute_sql2str(switch_mutex_t *mutex, char *sql, char *resbuf, size_t len);


/* Prototypes */
SWITCH_MODULE_SHUTDOWN_FUNCTION(test_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(test_runtime);
SWITCH_MODULE_LOAD_FUNCTION(test_load);


/* SWITCH_MODULE_DEFINITION(name, load, shutdown, runtime)
 * Defines a switch_loadable_module_function_table_t and a static const char[] modname
 */
SWITCH_MODULE_DEFINITION(mod_test, test_load, test_shutdown, NULL);


static switch_status_t do_config(switch_bool_t reload)
{
     
	switch_xml_t cfg = NULL, node = NULL, settings = NULL, param = NULL;
/*	char *data_dump;
	char *t_name;
	char *t_value=NULL;*/
	memset(&globals, 0, sizeof(globals));
	
        if ((cfg = switch_xml_open_cfg("test1.conf", &node, NULL))) {
                switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CRIT, " opened test.conf\n");
       		
		if((settings = switch_xml_child(node,"settings"))){
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
	
		
		}
	
	 }
	else{
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CRIT, "FAILED TO LOAD test.conf\n");
			switch_xml_free(node);
			return SWITCH_STATUS_TERM;
	}
	switch_xml_free(node);
       return SWITCH_STATUS_SUCCESS;

}


/* Module runtime function 
* 
*
 SWITCH_MODULE_RUNTIME_FUNCTION(test_runtime)
 {
 	while(globals.looping){
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"RUNTIME\n");
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CRIT, "FAILED TO LOAD test.conf\n");
	switch_yield(5000000);//wait for 5seconds
	}
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"EXIT RUNTIME\n");
        return SWITCH_STATUS_TERM;
		
 }
*
*
*/


/**
 * @Function Get db handler from configuartion and connect db mysql. 
 * @return DB Handler.
 */

static switch_cache_db_handle_t *switch_get_db_handler(void)
{
	switch_cache_db_handle_t *dbh = NULL;    /*! ODBC DSN HANDLER */
	char *dsn;                               /*! ODBC DSN STRING */

	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"WEL COME TO DBH\n");
	/**
	 * @Section Load DSN and from config
	 */
	
	if (!zstr(globals.odbc_dsn)) {
		dsn = globals.odbc_dsn;
	} else {
		dsn = globals.dbname;
	}

	
	/**
	 * @Function get odbc dsn db handler
	 */

	if (switch_cache_db_get_db_handle_dsn(&dbh, dsn) != SWITCH_STATUS_SUCCESS) {
		dbh = NULL;
	}

	return dbh;
}


/**
 * @Function Execute SQL with Callback Function 
 * @Return sql raw wise result 
 */

//static switch_bool_t switch_execute_sql_callback(switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback, void *pdata)
//{
//	switch_bool_t ret = SWITCH_FALSE;          /*! SQL Query Execution Status */
//	char *errmsg = NULL;                       /*! SQL Query Execution ERROR String */
//	switch_cache_db_handle_t *dbh = NULL;      /*! MySQL odbc dsn Handler */
//
//	/**
//	 * @Section Process Synchronisation
//	 */
//	
//	if (mutex) {
//		switch_mutex_lock(mutex);
//	} else {
//		switch_mutex_lock(globals.mutex);
//	}
//
//	/**
//	 * @Function get db handler dsn object
//	 */
//	
//	if (!(dbh = switch_get_db_handler())) {
//		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ ORIGINATION ] : Error Opening DB\n");
//		goto end;
//	}
//
//	/**
//	 * @Function Execute SQL in MYSQL and get result row wise in callback function
//	 */
//	
//	switch_cache_db_execute_sql_callback(dbh, sql, callback, pdata, &errmsg);
//	if (errmsg) {
//		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ ORIGINATION ] : SQL ERR: [%s] %s\n", sql, errmsg);
//		free(errmsg);
//	}
//
//	end:
//
//	/**
//	 * @Function release dsn handler 
//	 */
//	
//	switch_cache_db_release_db_handle(&dbh);
//
//	/**
//	 * @Section Process Synchronisation
//	 */
//	
//	if (mutex) {
//		switch_mutex_unlock(mutex);
//	} else {
//		switch_mutex_unlock(globals.mutex);
//	}
//
//	return ret;
//}


/**
 * @Function Execute MySQL Query and get result in string
 * @return resulting string
 */

static char *switch_execute_sql2str(switch_mutex_t *mutex, char *sql, char *resbuf, size_t len)
{
	char *ret = NULL;                          /*! SQL Query Result */
	switch_cache_db_handle_t *dbh = NULL;      /*! MySQL ODBC DSN Handler */   

	 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"WEL COME TO EXECUTE STR\n");
	/**
	 * @Section Process Synchronisation
	 */
	
	if (mutex) {
	 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"WEL COME TO EXECUTE STR1\n");
		switch_mutex_lock(mutex);
	} else {
	 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"WEL COME TO EXECUTE STR2\n");
		switch_mutex_lock(globals.mutex);
	}

	/**
	 * @Function get db handler dsn object
	 */
		
	if (!(dbh = switch_get_db_handler())) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ SWITCH ] : Error Opening DB\n");
		goto end;
	}

	/**
	 * @function Execute SQL in MySQL and get result.
	 */
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"EXECUTING QUERY\n");	
	ret = switch_cache_db_execute_sql2str(dbh, sql, resbuf, len, NULL);

	end:
	
	/**
	 * @function release db handler.
	 */
	
	switch_cache_db_release_db_handle(&dbh);

	/**
	 * @Section Process Synchronisation.
	 */
	
	if (mutex) {
		switch_mutex_unlock(mutex);
	} else {
		switch_mutex_unlock(globals.mutex);
	}

	//return SQL result.
	return ret;
}


static switch_status_t switch_execute_sql(char *sql, switch_mutex_t *mutex)
{
	switch_cache_db_handle_t *dbh = NULL;            /*! odbc dsn Handler */
	switch_status_t status = SWITCH_STATUS_FALSE;    /*! SQL Execution Status */

	/**
	 * @Section Process Synchronisation
	 */
	
	if (mutex) {
		switch_mutex_lock(mutex);
	} else {
		switch_mutex_lock(globals.mutex);
	}

	/**
	 * @Function get db handler dsn object
	 */
	
	if (!(dbh = switch_get_db_handler())) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "[ DIALPEER ] : Error Opening DB\n");
		goto end;
	}

	
	/**
	 * @function Execute SQL in MySQL and get result.
	 */
	
	status = switch_cache_db_execute_sql(dbh, sql, NULL);

	end:

	/**
	 * @function release db handler.
	 */

	switch_cache_db_release_db_handle(&dbh);

	/**
	 * @Section Process Synchronisation.
	 */
	
	if (mutex) {
		switch_mutex_unlock(mutex);
	} else {
		switch_mutex_unlock(globals.mutex);
	}

	return status;
}


SWITCH_STANDARD_APP(example_app)
{
	
	char *sql         = NULL;
	char result[50]  = "";	
	char *src_number  = NULL;
	char *dst_number  = NULL;
	char *call_id 	  = NULL;
	char *start_time  = NULL;
	//char *end_time    = NULL;
	int  duration     = 0;
	char *disposition = NULL;
	char *facility    = "mod_test";
	char *host_name   = "Ravindra";
	char *store_result = NULL;
	switch_channel_t *channel = NULL;
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"[MOD_TEST]:STARTING APP\n");
	
	/*      Get channel from Session     */
	channel = switch_core_session_get_channel(session);
	
	if(!channel){
		switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"[MOD_TEST]:unable to get channel..!!\n");
		goto end;
	}
	
	/*     get variables from channel     */
	src_number = (char*)switch_str_nil(switch_channel_get_variable(channel,"sip_from_user"));
	dst_number = (char*)switch_str_nil(switch_channel_get_variable(channel,"sip_to_user"));
	call_id = (char *)switch_str_nil(switch_channel_get_variable(channel, "uuid"));
	start_time = (char *)switch_str_nil(switch_channel_get_variable(channel, "created_time"));
	// end_time = (char *)switch_str_nil(switch_channel_get_variable(channel, "hangup_time"));
	disposition = (char *)switch_str_nil(switch_channel_get_variable(channel, "channel_name"));

	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"%s %s %s %s %s %s %s \n",src_number,dst_number,call_id,start_time,disposition,facility,host_name);	
	

	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"ADDING DATA TO DB\n");
	sql = switch_mprintf("insert into inmate_cdr (source_number,destination_number,call_id,start_time,end_time,duration,disposition,facility,host_name,created_at) values('%s','%s','%s',NOW(),NOW(),'%d','%s','%s','%s',NOW())",src_number,dst_number,call_id,start_time,duration,disposition,facility,host_name);
	//sql = switch_mprintf("INSERT INTO test (name,DATE) values ('%s',NOW())",host_name);
	switch_execute_sql(sql, globals.mutex);	
	switch_safe_free(sql);
	
	sql = switch_mprintf("select name from test limit 1");
	switch_execute_sql2str(NULL, sql, result, sizeof(result));
	switch_safe_free(sql);
	
	if(zstr(result)){
		switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"FAILED TO GET DATA FROM DB\n");
	}
	
	store_result = switch_mprintf("%s",result);
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"DATA recieved from db::: %s\n",store_result);

	sql = switch_mprintf("INSERT INTO test (name,DATE) values ('%s',NOW())",host_name);
        switch_execute_sql(sql, globals.mutex);       
        switch_safe_free(sql);


end:
	return;
}


SWITCH_MODULE_LOAD_FUNCTION(test_load)
{
	switch_status_t status;
//	switch_api_interface_t *api_interface;
	switch_application_interface_t *app_interface;

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

        switch_mutex_init(&globals.mutex, SWITCH_MUTEX_NESTED, globals.pool);



      if( (status = do_config(SWITCH_FALSE)) != SWITCH_STATUS_SUCCESS){
	 	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Do_config failed");
		return status;
	}else{       
		 switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Module configuration successfully loaded.\n");
	}

	SWITCH_ADD_APP(app_interface, "example" , "prints <num> log lines","prints <num> of log lines, 5 as default, min 0, max 1024", example_app, "<num>",SAF_NONE);	

        return SWITCH_STATUS_SUCCESS;
}


SWITCH_MODULE_SHUTDOWN_FUNCTION(test_shutdown)
{
        /* Cleanup dynamically allocated config settings */
  //      switch_xml_config_cleanup(instructions);
       	globals.looping=0;
	 return SWITCH_STATUS_SUCCESS;
}

