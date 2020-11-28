package cn.edm.model;

import java.util.Date;

import cn.edm.utils.file.Files;

/**
 * 文件. status(导入状态, 0:未导入, 1:已导入)
 * 
 * @author yjli
 */
public class File {

	private Integer fileId;
	private Integer corpId;
	private String userId;
	private String fileName;
	private String fileDesc;
	private String filePath;
	private String suff;
	private Integer fileSize;
	private String charset;
	private Integer emailCount;
	private Integer status;
	private Date createTime;
	private Date modifyTime;

	public String getSize() {
		return Files.unit(this.fileSize);
	}

	public Integer getFileId() {
		return fileId;
	}

	public void setFileId(Integer fileId) {
		this.fileId = fileId;
	}

	public Integer getCorpId() {
		return corpId;
	}

	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
	}

	public String getUserId() {
		return userId;
	}

	public void setUserId(String userId) {
		this.userId = userId;
	}

	public String getFileName() {
		return fileName;
	}

	public void setFileName(String fileName) {
		this.fileName = fileName;
	}

	public String getFileDesc() {
		return fileDesc;
	}

	public void setFileDesc(String fileDesc) {
		this.fileDesc = fileDesc;
	}

	public String getFilePath() {
		return filePath;
	}

	public void setFilePath(String filePath) {
		this.filePath = filePath;
	}

	public String getSuff() {
		return suff;
	}

	public void setSuff(String suff) {
		this.suff = suff;
	}

	public Integer getFileSize() {
		return fileSize;
	}

	public void setFileSize(Integer fileSize) {
		this.fileSize = fileSize;
	}

	public String getCharset() {
		return charset;
	}

	public void setCharset(String charset) {
		this.charset = charset;
	}

	public Integer getEmailCount() {
		return emailCount;
	}

	public void setEmailCount(Integer emailCount) {
		this.emailCount = emailCount;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public Date getModifyTime() {
		return modifyTime;
	}

	public void setModifyTime(Date modifyTime) {
		this.modifyTime = modifyTime;
	}

}
