package mini.webTest.client;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

public class IndexServlet extends HttpServlet {
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String method = request.getParameter("method");
        if (method.equalsIgnoreCase("getAll")) {
            getAll(request, response);
        } else if (method.equalsIgnoreCase("listBookWithCategory")) {
            listBookWithCategory(request, response);
        }
    }

    private void listBookWithCategory(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        //BusinessServiceImpl service = new BusinessServiceImpl();
        String category_id = request.getParameter("category_id");
        //List<Category> categories = service.getAllCategory();
        //request.setAttribute("categories", categories);
        String pagenum = request.getParameter("pagenum");
        //Page page = service.getBookPageData(pagenum, category_id);
        //request.setAttribute("page", page);
        request.getRequestDispatcher("/WEB-INF/jsp/body.jsp").forward(request, response);
    }

    private void getAll(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        //BusinessServiceImpl service = new BusinessServiceImpl();
        //List<Category> categories = service.getAllCategory();
        //request.setAttribute("categories", categories);
        String pagenum = request.getParameter("pagenum");
        //Page page = service.getBookPageData(pagenum);
        //request.setAttribute("page", page);
        request.getRequestDispatcher("/WEB-INF/jsp/body.jsp").forward(request, response);
    }

    public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        doGet(request, response);
    }
}
