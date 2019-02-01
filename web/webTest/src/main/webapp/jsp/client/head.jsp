<%@ page language="java" import="java.util.*" pageEncoding="UTF-8" %>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
    <title>首页头</title>
</head>

<body style="text-align:center;">
<h1>网上书店</h1>
<br/>
<div>
    <a href="${pageContext.request.contextPath }/client/IndexServlet?method=getAll" target="body">首页</a>
</div>

</body>
</html>
