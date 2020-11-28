package cn.edm.exception;

import org.springframework.security.core.AuthenticationException;

public class UserRoleException extends AuthenticationException{


	private static final long serialVersionUID = 7095663965953028425L;

	public UserRoleException(String msg) {
		super(msg);
		// TODO Auto-generated constructor stub
	}
	
	public UserRoleException(String msg, Throwable cause) {
		super(msg, cause);
	}


}
