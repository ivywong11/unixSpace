package mini.webTest.upload;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.*;
import java.nio.channels.FileChannel;
import java.util.*;

@WebServlet("/UploadActionServlet")
public class UploadActionServlet extends HttpServlet {
    private String serverPath = "h://upload";

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        System.out.println("into: UploadActionServlet");
        //获取action参数
        String action = request.getParameter("action");
        if ("mergeChunks".equals(action)) {
            // 获得需要合并的目录
            String fileMd5 = request.getParameter("fileMd5");
            // 读取目录下所有文件
            File file = new File(serverPath + "/" + fileMd5);
            File[] fileArray = file.listFiles(new FileFilter() {
                // 排除目录，只要文件
                @Override
                public boolean accept(File pathname) {
                    if (pathname.isDirectory()) {
                        return false;
                    }
                    return true;
                }
            });
            // 转成集合，便于排序
            List<File> fileList = new ArrayList<File>(Arrays.asList(fileArray));
            // 从小到大排序
            Collections.sort(fileList, new Comparator<File>() {
                @Override
                public int compare(File o1, File o2) {
                    if (Integer.parseInt(o1.getName()) < Integer.parseInt(o2.getName())) {
                        return -1;
                    }
                    return 1;
                }
            });
            // 新建保存文件
            File outputFile = new File(serverPath + "/" + UUID.randomUUID().toString() + ".zip");
            // 创建文件
            outputFile.createNewFile();
            // 输出流
            FileOutputStream fileOutputStream = new FileOutputStream(outputFile);
            FileChannel outChannel = fileOutputStream.getChannel();
            // 合并
            FileChannel inChannel;
            for (File a : fileList) {
                inChannel = new FileInputStream(a).getChannel();
                inChannel.transferTo(0, inChannel.size(), outChannel);
                inChannel.close();
                // 删除分片
                a.delete();
            }
            // 关闭流
            fileOutputStream.close();
            outChannel.close();
            // 清除文件加
            File tempFile = new File(serverPath + "/" + fileMd5);
            if (tempFile.isDirectory() && tempFile.exists()) {
                tempFile.delete();
            }
        } else if ("checkChunk".equals(action)) {
            // 校验文件是否已经上传并返回结果给前端
            // 文件唯一表示
            String fileMd5 = request.getParameter("fileMd5");
            // 当前分块下标
            String chunk = request.getParameter("chunk");
            // 当前分块大小
            String chunkSize = request.getParameter("chunkSize");
            response.setContentType("text/html;charset=utf-8");
            // 找到分块文件
            File checkFile = new File(serverPath + "/" + fileMd5 + "/" + chunk);
            // 检查文件是否存在，且大小一致
            if (checkFile.exists() && checkFile.length() == Integer.parseInt((chunkSize))) {
                response.getWriter().write("{\"ifExist\":1}");
            } else {
                response.getWriter().write("{\"ifExist\":0}");
            }
        }
    }

    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        doGet(request, response);
    }
}
