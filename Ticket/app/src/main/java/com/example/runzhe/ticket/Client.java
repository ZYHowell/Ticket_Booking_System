package com.example.runzhe.ticket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
//import java.net.ServerSocket;
import java.net.Socket;
//import java.net.URLEncoder;

//import java.net.InetAddress;
//import java.net.UnknownHostException;
//import java.util.Arrays;

import java.io.UnsupportedEncodingException;
//import sun.misc.*;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.util.Arrays;
import java.util.Scanner;

import sun.misc.BASE64Decoder;
import sun.misc.BASE64Encoder;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

@SuppressWarnings("unused")
public class Client
{
    public static Socket socket = new Socket();
    // public static Socket socket;
    public static OutputStream outputStream;
    public static InputStream inputStream;
//    private static final String host = "96.30.198.176";
    private static final String host = "59.78.38.126";
    private static final int port = 2333;
    private static final String key = "zhuangyonghaotql"; // 128 bit key
    private static final String initVector = "0000000000000000"; // 16 bytes IV

    public static void PREP() throws IOException
    {
        System.out.println("开始连接！");
        socket = new Socket(host, port);
        System.out.println("连接成功！");
    }

    /*public static String getUTF8(String _str)
    {
        StringBuffer sb = new StringBuffer();
        sb.append(_str);
        String str = "";
        String strUTF8 = "";
        try{
            str = new String(sb.toString().getBytes("UTF-8"));
            strUTF8 = URLEncoder.encode(str,"UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return strUTF8;
    }*/
    /*public static String getGBK(String _str) throws UnsupportedEncodingException
    {
        byte[] bytes = _str.getBytes("UTF-8");
        return (new String(bytes, "GBK"));
    }
    public static String getUTF8(String _str) throws UnsupportedEncodingException
    {
    	return new String(_str.getBytes("UTF-8"),"UTF-8");
    }*/
    /**
     * 将byte数组转化成String,为了支持中文，转化时用GBK编码方式
     */
    public static String ByteArraytoString(byte[] valArr,int maxLen) {
        String result=null;
        int index = 0;
        while(index < valArr.length && index < maxLen) {
            if(valArr[index] == 0) {
                break;
            }
            index++;
        }
        byte[] temp = new byte[index];
        System.arraycopy(valArr, 0, temp, 0, index);
        try {
            result= new String(temp,"GBK");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return result;
    }
    /**
     * 将String转化为byte,为了支持中文，转化时用GBK编码方式
     */
    public static byte[] StringToByteArray(String str){
        byte[] temp = null;
        try {
            temp = str.getBytes("GBK");
        } catch (UnsupportedEncodingException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return temp;
    }
    /*public static String FinalEncode(String _str)
    {
        BASE64Encoder encoder = new BASE64Encoder();
        byte[] b = _str.getBytes("UTF-8");
        String __str = encoder.encode(b);
        return __str;
    }

    public static String FinalDecode(String _str)
    {
        BASE64Decoder decoder = new BASE64Decoder();
        String __str = "";
        try {
        byte[] b = decoder.decodeBuffer(_str);
        __str = new String(b, "UTF-8");
        }
        catch (UnsupportedEncodingException e)
        {

        }
        return __str;
    }*/

    /**

     * 字符串转换成十六进制字符串
     */
   /* public static String str2HexStr(String str) {

        char[] chars = "0123456789ABCDEF".toCharArray();
        StringBuilder sb = new StringBuilder("");
        byte[] bs = str.getBytes();
        int bit;
        for (int i = 0; i < bs.length; i++) {
            bit = (bs[i] & 0x0f0) >> 4;
            sb.append(chars[bit]);
            bit = bs[i] & 0x0f;
            sb.append(chars[bit]);
        }
        return sb.toString();
    }*/

    /**
     *
     * 十六进制转换字符串
     */

    /*public static byte[] hexStr2Bytes(String hexStr) {
        System.out.println("in len :" + hexStr.length());
        String str = "0123456789ABCDEF";
        char[] hexs = hexStr.toCharArray();
        byte[] bytes = new byte[hexStr.length() / 2];
        int n;
        for (int i = 0; i < bytes.length; i++) {
            n = str.indexOf(hexs[2 * i]) * 16;
            n += str.indexOf(hexs[2 * i + 1]);
            bytes[i] = (byte) (n & 0xff);
        }
        System.out.println("out len :" + bytes.length);
        System.out.println("ddd" + Arrays.toString(bytes));
        return bytes;
    }*/

    /**
     * bytes转换成十六进制字符串
     */
    /*public static String byte2HexStr(byte[] b) {
        String hs = "";
        String stmp = "";
        for (int n = 0; n < b.length; n++) {
            stmp = (Integer.toHexString(b[n] & 0XFF));
            if (stmp.length() == 1)
                hs = hs + "0" + stmp;
            else
                hs = hs + stmp;
            // if (n<b.length-1) hs=hs+":";
        }
        return hs.toUpperCase();
    }*/
	/*public static String Encrypt(String key, String initVector, String value)
	{
		try {
			IvParameterSpec iv = new IvParameterSpec(initVector.getBytes("UTF-8"));
			SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes("UTF-8"), "AES");

			Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
			cipher.init(Cipher.ENCRYPT_MODE, skeySpec, iv);

			byte[] encrypted = cipher.doFinal(value.getBytes());
			return byte2HexStr(encrypted);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return null;
	}*/
	/*public static String Decrypt(String key, String initVector, String encrypted)
	{
		try {
			IvParameterSpec iv = new IvParameterSpec(initVector.getBytes("UTF-8"));
			SecretKeySpec skeySpec = new SecretKeySpec(key.getBytes("UTF-8"), "AES");

			Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
			cipher.init(Cipher.DECRYPT_MODE, skeySpec, iv);

			byte[] original = cipher.doFinal(hexStr2Bytes(encrypted));
			return new String(original);
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		return null;
	}*/
	/*
    // 加密
    public static String Encrypt(String sSrc, String sKey) throws Exception {
        if (sKey == null) {
            System.out.print("Key为空null");
            return null;
        }
        // 判断Key是否为16位
        if (sKey.length() != 16) {
            System.out.print("Key长度不是16位");
            return null;
        }
        byte[] raw = sKey.getBytes("utf-8");
        SecretKeySpec skeySpec = new SecretKeySpec(raw, "AES");
        Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");//"算法/模式/补码方式"
        cipher.init(Cipher.ENCRYPT_MODE, skeySpec);
        byte[] encrypted = cipher.doFinal(sSrc.getBytes("utf-8"));

        //return new String(encrypted, "utf-8");
        return new Base64().encodeToString(encrypted);//此处使用BASE64做转码功能，同时能起到2次加密的作用。
    }

    // 解密
    public static String Decrypt(String sSrc, String sKey) throws Exception {
        try {
            // 判断Key是否正确
            if (sKey == null) {
                System.out.print("Key为空null");
                return null;
            }
            // 判断Key是否为16位
            if (sKey.length() != 16) {
                System.out.print("Key长度不是16位");
                return null;
            }
            byte[] raw = sKey.getBytes("utf-8");
            SecretKeySpec skeySpec = new SecretKeySpec(raw, "AES");
            Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
            cipher.init(Cipher.DECRYPT_MODE, skeySpec);
            //byte[] encrypted1 = sSrc.getBytes("utf-8");
            byte[] encrypted1 = new Base64().decode(sSrc);//先用base64解密
            try {
                byte[] original = cipher.doFinal(encrypted1);
                String originalString = new String(original,"utf-8");
                return originalString;
            } catch (Exception e) {
                System.out.println(e.toString());
                return null;
            }
        } catch (Exception ex) {
            System.out.println(ex.toString());
            return null;
        }
    }*/

    /**
     * 加密算法
     */
    private static final String ENCRY_ALGORITHM = "AES";

    /**
     * 加密算法/加密模式/填充类型
     * 本例采用AES加密，ECB加密模式，PKCS5Padding填充
     */
    private static final String CIPHER_MODE = "AES/ECB/PKCS5Padding";

    /**
     * 设置iv偏移量
     * 本例采用ECB加密模式，不需要设置iv偏移量
     */
    private static final String IV_ = null;

    /**
     * 设置加密字符集
     * 本例采用 UTF-8 字符集
     */
    private static final String CHARACTER = "UTF-8";

    /**
     * 设置加密密码处理长度。
     * 不足此长度补0；
     */
    private static final int PWD_SIZE = 16;

    /**
     * 密码处理方法
     * 如果加解密出问题，
     * 请先查看本方法，排除密码长度不足补"0",导致密码不一致
     * @param password 待处理的密码
     * @return
     * @throws UnsupportedEncodingException
     */
    private static byte[] pwdHandler(String password) throws UnsupportedEncodingException {
        byte[] data = null;
        if (password == null) {
            password = "";
        }
        StringBuffer sb = new StringBuffer(PWD_SIZE);
        sb.append(password);
        while (sb.length() < PWD_SIZE) {
            sb.append("0");
        }
        if (sb.length() > PWD_SIZE) {
            sb.setLength(PWD_SIZE);
        }

        data = sb.toString().getBytes("UTF-8");

        return data;
    }

    //======================>原始加密<======================

    /**
     * 原始加密
     * @param clearTextBytes 明文字节数组，待加密的字节数组
     * @param pwdBytes 加密密码字节数组
     * @return 返回加密后的密文字节数组，加密错误返回null
     */
    public static byte[] encrypt(byte[] clearTextBytes, byte[] pwdBytes) {
        try {
            // 1 获取加密密钥
            SecretKeySpec keySpec = new SecretKeySpec(pwdBytes, ENCRY_ALGORITHM);

            // 2 获取Cipher实例
            Cipher cipher = Cipher.getInstance(CIPHER_MODE);

            // 查看数据块位数 默认为16（byte） * 8 =128 bit
//	            System.out.println("数据块位数(byte)：" + cipher.getBlockSize());

            // 3 初始化Cipher实例。设置执行模式以及加密密钥
            cipher.init(Cipher.ENCRYPT_MODE, keySpec);

            // 4 执行
            byte[] cipherTextBytes = cipher.doFinal(clearTextBytes);

            // 5 返回密文字符集
            return cipherTextBytes;

        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (BadPaddingException e) {
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    /**
     * 原始解密
     * @param cipherTextBytes 密文字节数组，待解密的字节数组
     * @param pwdBytes 解密密码字节数组
     * @return 返回解密后的明文字节数组，解密错误返回null
     */
    public static byte[] decrypt(byte[] cipherTextBytes, byte[] pwdBytes) {

        try {
            // 1 获取解密密钥
            SecretKeySpec keySpec = new SecretKeySpec(pwdBytes, ENCRY_ALGORITHM);

            // 2 获取Cipher实例
            Cipher cipher = Cipher.getInstance(CIPHER_MODE);

            // 查看数据块位数 默认为16（byte） * 8 =128 bit
//	            System.out.println("数据块位数(byte)：" + cipher.getBlockSize());

            // 3 初始化Cipher实例。设置执行模式以及加密密钥
            cipher.init(Cipher.DECRYPT_MODE, keySpec);

            // 4 执行
            byte[] clearTextBytes = cipher.doFinal(cipherTextBytes);

            // 5 返回明文字符集
            return clearTextBytes;

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (BadPaddingException e) {
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        // 解密错误 返回null
        return null;
    }

    //======================>BASE64<======================

    /**
     * BASE64加密
     * @param clearText 明文，待加密的内容
     * @param password 密码，加密的密码
     * @return 返回密文，加密后得到的内容。加密错误返回null
     */
    public static String encryptBase64(String clearText, String password) {
        try {
            // 1 获取加密密文字节数组
            byte[] cipherTextBytes = encrypt(clearText.getBytes(CHARACTER), pwdHandler(password));

            // 2 对密文字节数组进行BASE64 encoder 得到 BASE6输出的密文
            BASE64Encoder base64Encoder = new BASE64Encoder();
            String cipherText = base64Encoder.encode(cipherTextBytes);

            // 3 返回BASE64输出的密文
            return cipherText;
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        // 加密错误 返回null
        return null;
    }

    /**
     * BASE64解密
     * @param cipherText 密文，带解密的内容
     * @param password 密码，解密的密码
     * @return 返回明文，解密后得到的内容。解密错误返回null
     */


    public static String decryptBase64(String cipherText, String password) {
        try {
            // 1 对 BASE64输出的密文进行BASE64 decodebuffer 得到密文字节数组
            BASE64Decoder base64Decoder = new BASE64Decoder();
            byte[] cipherTextBytes = base64Decoder.decodeBuffer(cipherText);

            // 2 对密文字节数组进行解密 得到明文字节数组
            byte[] clearTextBytes = decrypt(cipherTextBytes, pwdHandler(password));

            // 3 根据 CHARACTER 转码，返回明文字符串
            return new String(clearTextBytes, CHARACTER);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        // 解密错误返回null
        return null;
    }

    //======================>HEX<======================

    /**
     * HEX加密
     * @param clearText 明文，待加密的内容
     * @param password 密码，加密的密码
     * @return 返回密文，加密后得到的内容。加密错误返回null
     */
    public static String encryptHex(String clearText, String password) {
        try {
            // 1 获取加密密文字节数组
            byte[] cipherTextBytes = encrypt(clearText.getBytes(CHARACTER), pwdHandler(password));

            // 2 对密文字节数组进行 转换为 HEX输出密文
            String cipherText = byte2hex(cipherTextBytes);

            // 3 返回 HEX输出密文
            return cipherText;
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        // 加密错误返回null
        return null;
    }

    /**
     * HEX解密
     * @param cipherText 密文，带解密的内容
     * @param password 密码，解密的密码
     * @return 返回明文，解密后得到的内容。解密错误返回null
     */
    public static String decryptHex(String cipherText, String password) {
        try {
            // 1 将HEX输出密文 转为密文字节数组
            byte[] cipherTextBytes = hex2byte(cipherText);

            // 2 将密文字节数组进行解密 得到明文字节数组
            byte[] clearTextBytes = decrypt(cipherTextBytes, pwdHandler(password));

            // 3 根据 CHARACTER 转码，返回明文字符串
            return new String(clearTextBytes, CHARACTER);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        // 解密错误返回null
        return null;
    }

    /*字节数组转成16进制字符串  */
    public static String byte2hex(byte[] bytes) { // 一个字节的数，
        StringBuffer sb = new StringBuffer(bytes.length * 2);
        String tmp = "";
        for (int n = 0; n < bytes.length; n++) {
            // 整数转成十六进制表示
            tmp = (java.lang.Integer.toHexString(bytes[n] & 0XFF));
            if (tmp.length() == 1) {
                sb.append("0");
            }
            sb.append(tmp);
        }
        return sb.toString().toUpperCase(); // 转成大写
    }

    /*将hex字符串转换成字节数组 */
    private static byte[] hex2byte(String str) {
        if (str == null || str.length() < 2) {
            return new byte[0];
        }
        str = str.toLowerCase();
        int l = str.length() / 2;
        byte[] result = new byte[l];
        for (int i = 0; i < l; ++i) {
            String tmp = str.substring(2 * i, 2 * i + 2);
            result[i] = (byte) (Integer.parseInt(tmp, 16) & 0xFF);
        }
        return result;
    }

    /**
     * STRING TO UTF-8
     */
    public static String convertStringToUTF8(String s) {
        if (s == null || s.equals("")) {
            return null;
        }
        StringBuffer sb = new StringBuffer();
        try {
            char c;
            for (int i = 0; i < s.length(); i++) {
                c = s.charAt(i);
                if (c >= 0 && c <= 255) {
                    sb.append(c);
                } else {
                    byte[] b;
                    b = Character.toString(c).getBytes("utf-8");
                    for (int j = 0; j < b.length; j++) {
                        int k = b[j];
                        //转换为unsigned integer  无符号integer
						/*if (k < 0)
							k += 256;*/
                        k = k < 0? k+256:k;
                        //返回整数参数的字符串表示形式 作为十六进制（base16）中的无符号整数
                        //该值以十六进制（base16）转换为ASCII数字的字符串
                        sb.append(Integer.toHexString(k).toUpperCase());

                        // url转置形式
                        // sb.append("%" +Integer.toHexString(k).toUpperCase());
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return sb.toString();
    }
    /**
     * UTF-8 TO STRING
     */
    public static String convertUTF8ToString(String s) {
        if (s == null || s.equals("")) {
            return null;
        }
        try {
            s = s.toUpperCase();
            int total = s.length() / 2;
            //标识字节长度
            int pos = 0;
            byte[] buffer = new byte[total];
            for (int i = 0; i < total; i++) {
                int start = i * 2;
                //将字符串参数解析为第二个参数指定的基数中的有符号整数。
                buffer[i] = (byte) Integer.parseInt(s.substring(start, start + 2), 16);
                pos++;
            }
            //通过使用指定的字符集解码指定的字节子阵列来构造一个新的字符串。
            //新字符串的长度是字符集的函数，因此可能不等于子数组的长度。
            return new String(buffer, 0, pos, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return s;
    }
    public static String SEND(String _message) throws Exception
    {
        //向服务端发送数据
        outputStream = socket.getOutputStream();
        //修正_message
        //_message = convertStringToUTF8(_message);

        System.out.println("Origin : "+_message);

        String message = "#*#";
        //_message = Encrypt(key, initVector, _message);
        byte[] _mes_bytes = StringToByteArray(_message);
        _message = new String(_mes_bytes, "GBK");
        //_message = Encrypt(key, initVector, _message);
        //_message = Encrypt(_message, key);encryptBase64
        _message = encryptBase64(_message, key);

        //int _message_len = _message.length();
        message = message + _message + "#?#";
        //byte[] sen_bytes = StringToByteArray(message);

        //message = FinalEncode(message); //ENCODE

        outputStream.write(message.getBytes("UTF-8"));
        //outputStream.write(StringToByteArray(message));
        System.out.println("message: "+message);
        outputStream.flush();
        //终止发送数据，客户端现在只接收数据
        //如果不加shutdownOutput，服务器端一直阻塞在read()方法中，导致客户端无法收到服务端回显的数据
        //shutdownOutput只关闭客户端向服务端的输出流，并不会关闭整个Socket的连接
        socket.shutdownOutput();

        //从服务端接收数据
        inputStream = socket.getInputStream();
        byte[] bytes = new byte[1024];
        int len = 0;
        StringBuffer sb = new StringBuffer();
        System.out.println("START RECEIVING!");
        while ((len = inputStream.read(bytes)) != -1)
        {
            sb.append(new String(bytes, 0, len, "UTF-8"));
            //sb.append(ByteArraytoString(bytes, bytes.length));
            System.out.println(len);
            //System.out.println("客户端正在从输入流中读数据");
        }
        System.out.println("来自服务端的回显:" + sb.toString());

        //process the sb received
        int p_st = sb.indexOf("#*#") + 3;
        //int p_en = sb.indexOf("#",p_st);
        //String len_message_ = sb.substring(p_st,p_en);
        //System.out.println("back len_message : "+len_message_);
        //int len_message = Integer.valueOf(len_message_);
        int p_en = sb.indexOf("#?#");
        String _recv_m = sb.substring(p_st,p_en);
        System.out.println("before decryption : "+_recv_m);
        //String _recv_message = Decrypt(_recv_m, key);
        String _recv_message = decryptBase64(_recv_m, key);
        System.out.println("AFTER decryption : "+_recv_message);

        //_recv_message = convertUTF8ToString(_recv_message);
        //byte[] _recv_bytes = _recv_message.getBytes("UTF-8");
        //_recv_message = ByteArraytoString(_recv_bytes, _recv_bytes.length);
        System.out.println("final decryption : "+_recv_message);
        //process _recv_m
        //String _recv_message = Decrypt(key, initVector, _recv_m);
        //String _recv_message = _recv_m;

        return _recv_message;
    }

    public static void END_SOCKET() throws IOException
    {
        //调用close方法，会直接关闭整个Socket连接
        outputStream.close();
        inputStream.close();
        socket.close();
        System.out.println("END THIS SOCKET!");
    }

    public static String fuck(String outmes) throws Exception
    {
        String backmes = "";

        System.out.println("-> outmes : "+outmes);
        PREP();
        backmes = SEND(outmes);
        System.out.println("-> backmes : "+backmes);
        END_SOCKET();

        return backmes;
    }
}
