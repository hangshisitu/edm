package cn.edm.security;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.web.authentication.AuthenticationFailureHandler;

import cn.edm.constant.ErrorCode;
import cn.edm.constant.Sessions;

/**
 * 登录验证出错处理
 * @author Luxh
 */
public class LoginAuthenticationFailureHandler implements AuthenticationFailureHandler {

	@Override
	public void onAuthenticationFailure(HttpServletRequest request,
			HttpServletResponse response, AuthenticationException ae)
			throws IOException, ServletException {
		ae.printStackTrace();
		String errorMessage = ae.getMessage();
		String errorCode = ErrorCode.USERNAME_PASSWORD_NOT_CORRECT;
		if(ErrorCode.USERNAME_OR_PASSWORD_NOT_CORRECT_MESSAGE.equals(errorMessage)) {
			errorCode = ErrorCode.USERNAME_PASSWORD_NOT_CORRECT;
		}else if(ErrorCode.USER_STATUS_INVALID__MESSAGE.equals(errorMessage)) {
			errorCode = ErrorCode.USER_STATUS_INVALID;
		}else if(ErrorCode.NOT_ADMIN_ROLE_MESSAGE.equals(errorMessage)) {
			errorCode = ErrorCode.NOT_ADMIN_ROLE;
		}else if(ae instanceof BadCredentialsException){
			int errorCount =  (request.getSession().getAttribute(Sessions.PASSWORD_ERROR_COUNT)==null?0:(Integer)request.getSession().getAttribute(Sessions.PASSWORD_ERROR_COUNT));
			if(errorCount<5) {
				errorCount += 1;
				request.getSession().setAttribute(Sessions.PASSWORD_ERROR_COUNT, errorCount);
			}
		}
		response.sendRedirect(request.getContextPath()+"/login?error="+errorCode);
	}

}
