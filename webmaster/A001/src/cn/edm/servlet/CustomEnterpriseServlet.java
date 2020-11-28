package cn.edm.servlet;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import net.sf.json.JSONObject;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.FileItemFactory;
import org.apache.commons.fileupload.FileUploadException;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;
import org.springframework.stereotype.Component;

import cn.edm.constant.Config;
import cn.edm.constants.Status;
import cn.edm.domain.CustomEnterprise;
import cn.edm.modules.utils.Props;
import cn.edm.service.CustomEnterpriseService;
import cn.edm.util.BeanUtils;

@WebServlet("/custom/save")
@Component
public class CustomEnterpriseServlet extends HttpServlet{

	private static final long serialVersionUID = 4282692049133607967L;

    protected void service(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {  
        try {  
            PrintWriter out = response.getWriter();  
            // 文件保存目录路径  
            /*String savePath = request.getServletContext().getRealPath("/")+ "/resources/attached/";*/ 
            String savePath = Props.getStr(Config.NGINX_PATH)  + "/resources/logo/";
            // 文件保存目录URL  
            /*String saveUrl = request.getContextPath() + "/resources/logo/"; */ 
            String saveUrl = "resources/logo/";
            // 定义允许上传的文件扩展名  
            HashMap<String, String> extMap = new HashMap<String, String>();  
            extMap.put("image", "gif,jpg,jpeg,png,bmp");  
            // 最大文件大小  
            long maxSize = 1000000;  
            response.setContentType("text/html; charset=UTF-8");  
  
            if (!ServletFileUpload.isMultipartContent(request)) {  
                out.println(getError("请选择文件。"));  
                return;  
            }  
            // 检查目录  
            File uploadDir = new File(savePath);  
            if (!uploadDir.isDirectory()) {  
/*                out.println(getError("上传目录不存在。"));  
                return;  */
            	uploadDir.mkdirs();
            }
            
            // 检查目录写权限  
            if (!uploadDir.canWrite()) {  
                out.println(getError("上传目录没有写权限。"));  
                return;  
            }  
  
            String dirName = request.getParameter("dir");  
            if (dirName == null) {  
                dirName = "image";  
            }  
            if (!extMap.containsKey(dirName)) {  
                out.println(getError("目录名不正确。"));  
                return;  
            }  
            // 创建文件夹  
            savePath += dirName + "/";  
            saveUrl += dirName + "/";  
            File saveDirFile = new File(savePath);  
            if (!saveDirFile.exists()) {  
                saveDirFile.mkdirs();  
            }  
            SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd");  
            String ymd = sdf.format(new Date());  
            savePath += ymd + "/";  
            saveUrl += ymd + "/";  
            File dirFile = new File(savePath);  
            if (!dirFile.exists()) {  
                dirFile.mkdirs();  
            }  
  
            FileItemFactory factory = new DiskFileItemFactory();  
            ServletFileUpload upload = new ServletFileUpload(factory);  
            upload.setHeaderEncoding("UTF-8");  
            List items = upload.parseRequest(request);  
            Iterator itr = items.iterator();  
            while (itr.hasNext()) {  
                FileItem item = (FileItem) itr.next();  
                String fileName = item.getName();  
                if (!item.isFormField()) {  
                    // 检查文件大小  
                    if (item.getSize() > maxSize) {  
                        out.println(getError("上传文件大小超过限制。"));  
                        return;  
                    }  
                    // 检查扩展名  
                    String fileExt = fileName.substring(fileName.lastIndexOf(".") + 1).toLowerCase();  
                    if (!Arrays.<String> asList(extMap.get(dirName).split(",")).contains(fileExt)) {  
                        out.println(getError("上传文件扩展名是不允许的扩展名。\n只允许"    + extMap.get(dirName) + "格式。"));  
                        return;  
                    }  
  
                    SimpleDateFormat df = new SimpleDateFormat("yyyyMMddHHmmss");  
                    String newFileName = df.format(new Date()) + "_" + new Random().nextInt(1000) + "." + fileExt;  
                    try {  
                        File uploadedFile = new File(savePath, newFileName);  
                        item.write(uploadedFile);  
                    } catch (Exception e) {  
                        out.println(getError("上传文件失败。"));  
                        return;  
                    }  
                    
                   
                    CustomEnterprise customLogo = new CustomEnterprise();
                    customLogo.setStatus(Status.DISABLED);
                    customLogo.setLogoUrl(saveUrl + newFileName);
                    customLogo.setCreateTime(new Date());
                    
                    CustomEnterpriseService customEnterpriseService = (CustomEnterpriseService) BeanUtils.getBean("customEnterpriseService",getServletContext());
                    customEnterpriseService.save(customLogo);
                    
                    JSONObject obj = new JSONObject();  
                    obj.put("error", 0);  
                    obj.put("message","新建企业logo成功,如使用请激活");
                    obj.put("url", saveUrl + newFileName);  
                    out.println(obj.toString());  
                }  
            }  
        } catch (FileUploadException e1) {  
            e1.printStackTrace();  
        }  
  
    }  
     
    private String getError(String message) {  
        JSONObject obj = new JSONObject();  
        obj.put("error", 1);  
        obj.put("message", message);  
        return obj.toString();  
    }

    
}
