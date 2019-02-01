<%@ page language="java" import="java.util.*" pageEncoding="UTF-8" %>
<%@page import="java.io.File" %>
<%@page import="java.io.BufferedReader" %>
<%@page import="java.io.FileReader" %>
<%@page import="java.io.InputStreamReader" %>
<%@page import="java.io.FileInputStream" %>
<%@page import="mini.webTest.up6.*" %>
<%@page import="mini.webTest.up6.DbHelper" %>
<%@page import="mini.webTest.up6.PathTool" %>
<%
    //path = ""
    String path = request.getContextPath();
    //basePath = "http://localhost:80/"
    String basePath = request.getScheme() + "://" + request.getServerName() + ":" + request.getServerPort() + path + "/";
    //E:\IdeaProjects\webTest\target\webTest-1.0-SNAPSHOT\jsp\db\sql.jsp
    String pathCur = application.getRealPath(request.getServletPath());
    //E:\IdeaProjects\webTest\target\webTest-1.0-SNAPSHOT\jsp\db
    String pathParent = new File(pathCur).getParent();
    //E:\IdeaProjects\webTest\target\webTest-1.0-SNAPSHOT\jsp
    pathParent = new File(pathParent).getParent();
    //E:\IdeaProjects\webTest\target\webTest-1.0-SNAPSHOT
    pathParent = new File(pathParent).getParent();
    //E:\IdeaProjects\webTest\target\webTest-1.0-SNAPSHOT\sql
    String sqlDir = PathTool.combine(pathParent, "sql");
    //E:\IdeaProjects\webTest\target\webTest-1.0-SNAPSHOT\sql.down
    String downDir = PathTool.combine(pathParent, "sql.down");
    DbHelper db = new DbHelper();
    String[] clear_type = {"ARRAY_MD5"};
    //select count(*) from user_objects where object_type='TYPE' and object_name = ARRAY_MD5
    for (String str : clear_type) {
        String sql = "select count(*) from user_objects where object_type='TYPE' and object_name = '" + str + "'";
        int i = db.ExecuteScalar(sql);
        if (i > 0) {
            //DROP TYPE ARRAY_MD5
            String sql_drop = "DROP TYPE " + str;
            db.ExecuteNonQuery(sql_drop);
        }
    }
    String[] clear_table = {"UP6_FILES", "UP6_FOLDERS", "DOWN_FILES", "DOWN_FOLDERS"};
    for (String str : clear_table) {
        String sql = "select count(*) from user_tables where table_name = '" + str + "'";
        int i = db.ExecuteScalar(sql);
        if (i > 0) {
            String sql_drop = "DROP TABLE " + str;
            db.ExecuteNonQuery(sql_drop);
        }
    }
    String[] clear_procedure = {"FD_FILES_CHECK", "FD_ADD_BATCH"};
    for (String str : clear_procedure) {
        String sql = "select count(*) from user_objects where object_type='PROCEDURE' and object_name = '" + str + "'";
        int i = db.ExecuteScalar(sql);
        if (i > 0) {
            String sql_drop = "DROP PROCEDURE " + str;
            db.ExecuteNonQuery(sql_drop);
        }
    }
    File dir = new File(sqlDir);
    if (dir.exists()) {
        File[] files = dir.listFiles();
        if (files.length > 0) {
            for (File file : files) {
                if (file.getName().endsWith(".sql")) {
                    InputStreamReader isr = new InputStreamReader(new FileInputStream(file), "UTF-8");
                    BufferedReader reader = new BufferedReader(isr);
                    StringBuffer buffer = new StringBuffer();
                    String text;
                    while ((text = reader.readLine()) != null) {
                        buffer.append(text + "\n");
                    }
                    String sb = buffer.toString();
                    db.ExecuteNonQuery(sb);
                    reader.close();
                    isr.close();
                    //XDebug.Output("sql",sb);
                    XDebug.Output("sql", file.getName());
                }
            }
        }
    }
%>


